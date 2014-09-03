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


Physics_Character_Controller::~Physics_Character_Controller()
{
  std::cout << "Deleting character controller" << std::endl;
}


void Physics_Character_Controller::bullet_character_step(const double dt)
{
  btPairCachingGhostObject *ghost = getGhostObject();
  glm::mat4 m;

  if (!node)
    return;
  
  ghost->getWorldTransform().getOpenGLMatrix((btScalar *) &m);
 // node->transform_model_set(m);

  btScalar walkVelocity = 15.0;
  btScalar walkSpeed = walkVelocity * dt;
  float rotation_speed = 3.f * (float) dt;

  if (direction & PHYSICS_DIRECTION_LEFT) {
  //  glm::mat4 model = glm::rotate(node->transform_model_get(), rotation_speed, glm::vec3(0, 1, 0));
  //  node->transform_model_set(model);
    rotation_angle += rotation_speed;
  }
  if (direction & PHYSICS_DIRECTION_RIGHT) {
  //  glm::mat4 model = glm::rotate(node->transform_model_get(), -rotation_speed, glm::vec3(0, 1, 0));
   // node->transform_model_set(model);
    rotation_angle -= rotation_speed;
  }

  if (direction & PHYSICS_DIRECTION_ROTATE) {
  //  glm::mat4 model = glm::rotate(node->transform_model_get(), -rotation_angle + angle_joystick, glm::vec3(0, 1, 0));
  //  node->transform_model_set(model);
    rotation_angle = angle_joystick;
  }

  /*
  glm::mat4 model = node->transform_model_get();
  glm::vec3 forward = glm::vec3(model[2]);
  glm::vec3 strafe = glm::vec3(model[0]);
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
  ghost->getWorldTransform().setFromOpenGLMatrix((btScalar *) &node->transform_model_get());
  */
}


void  Physics_Character_Controller::bullet_debug_draw_contacts(btDiscreteDynamicsWorld *world, btBroadphaseInterface *broadphase)
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


void Physics_Character_Controller::joystick_angle_set(const float angle)
{
  angle_joystick = angle;
}


void Physics_Character_Controller::reset()
{
  state = CHARACTER_STATE_IDLE;
  direction = PHYSICS_DIRECTION_NONE;
  rotation_angle = (float) M_PI / 2.f;
  setJumpSpeed(15);
}


void Physics_Character_Controller::node_set(Node &node)
{
  this->node = &node;
}


void Physics_Character_Controller::move(Physics_Direction direction)
{
  this->direction = direction;

}


const Physics_Character_State  Physics_Character_Controller::state_get()
{
  if (!onGround()) {
    state = CHARACTER_STATE_JUMPING;
  } else if (!m_walkDirection.isZero()) {
    state = CHARACTER_STATE_MOVING;
  } else {
    state = CHARACTER_STATE_IDLE;
  }

  return state;
}
