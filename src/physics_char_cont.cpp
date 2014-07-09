#include "physics_char_cont.h"
#include <btBulletDynamicsCommon.h>
#include <LinearMath/btIDebugDraw.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletCollision/CollisionShapes/btMultiSphereShape.h>
#include <BulletCollision/BroadphaseCollision/btOverlappingPairCache.h>
#include <BulletCollision/BroadphaseCollision/btCollisionAlgorithm.h>
#include <BulletCollision/CollisionDispatch/btCollisionWorld.h>
#include <LinearMath/btDefaultMotionState.h>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "utils.h"


Physics_CharacterController::~Physics_CharacterController()
{
  std::cout << "Deleting character controller" << std::endl;
}


void Physics_CharacterController::bullet_character_step()
{
  btPairCachingGhostObject *ghost = getGhostObject();
  glm::mat4 m;

  if (!node)
    return;
  
  ghost->getWorldTransform().getOpenGLMatrix((btScalar *) &m);
  node->mesh->model = m;

  btScalar walkVelocity = btScalar(1.1) * 3.0; // 4 km/h -> 1.1 m/s
  btScalar walkSpeed = walkVelocity * 0.16;

  if (direction & PHYSICS_DIRECTION_LEFT) {
    node->mesh->model = glm::rotate(node->mesh->model, 0.03f, glm::vec3(0, 1, 0));
    rotation_angle += 0.03f;
  }
  if (direction & PHYSICS_DIRECTION_RIGHT) {
    node->mesh->model = glm::rotate(node->mesh->model, -0.03f, glm::vec3(0, 1, 0));
    rotation_angle -= 0.03f;
  }

  if (direction & PHYSICS_DIRECTION_ROTATE) {
    node->mesh->model = glm::rotate(node->mesh->model, -rotation_angle + angle_joystick, glm::vec3(0, 1, 0));
    rotation_angle = angle_joystick;
  }

  glm::vec3 forward = glm::vec3(node->mesh->model[2]);
  glm::vec3 strafe = glm::vec3(node->mesh->model[0]);
  glm::vec3 v(0.f, 0.f, 0.f);

  if (direction & PHYSICS_DIRECTION_FORWARD) {
    v += forward;
  }
  if (direction & PHYSICS_DIRECTION_BACK) {
    v -= forward;
  }
  if (direction & PHYSICS_DIRECTION_STRAFE_LEFT) {
    v += strafe;
  }
  if (direction & PHYSICS_DIRECTION_STRAFE_RIGHT) {
    v -= strafe;
  }
  glm::normalize(v);

  btVector3 walkDirection(v.x, v.y, v.z);
  setWalkDirection(walkDirection * walkSpeed);
  ghost->getWorldTransform().setFromOpenGLMatrix((btScalar *) &node->mesh->model);
}


void  Physics_CharacterController::bullet_debug_draw_contacts(btDiscreteDynamicsWorld *world, btBroadphaseInterface *broadphase)
{
  btManifoldArray manifoldArray;
  btBroadphasePairArray& pairArray = m_ghostObject->getOverlappingPairCache()->getOverlappingPairArray();
  int numPairs = pairArray.size();

  for (int i = 0; i < numPairs; i++) {
    manifoldArray.clear();
    const btBroadphasePair& pair = pairArray[i];

    btBroadphasePair *collisionPair = broadphase->getOverlappingPairCache()->findPair(pair.m_pProxy0,pair.m_pProxy1);
    if (!collisionPair)
      continue;

    if (collisionPair->m_algorithm)
      collisionPair->m_algorithm->getAllContactManifolds(manifoldArray);

    for (int j = 0; j < manifoldArray.size(); j++) {
      btPersistentManifold *manifold = manifoldArray[j];
      for (int p = 0; p < manifold->getNumContacts(); p++) {
        const btManifoldPoint&pt = manifold->getContactPoint(p);
        btVector3 color(0, 255, 0);
        world->getDebugDrawer()->drawContactPoint(pt.getPositionWorldOnB(),pt.m_normalWorldOnB,pt.getDistance(),pt.getLifeTime(),color);
      }
    }
  }

}


void Physics_CharacterController::defaults_set()
{
  direction = PHYSICS_DIRECTION_NONE;
  rotation_angle = (float) M_PI / 2.f;
  setJumpSpeed(15);
}


void Physics_CharacterController::node_set(Node &node)
{
  this->node = &node;
}


void Physics_CharacterController::move(Physics_Direction direction)
{
  this->direction = direction;

}

