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

  std::cout << "Starting simulation" << std::endl;
  btTransform xform;

  glm::vec3 new_x(node->mesh->model[0]);
  glm::vec3 new_y(node->mesh->model[1]);
  glm::vec3 new_z(node->mesh->model[2]);
  std::cout << glm::to_string(new_x) << std::endl;
  std::cout << glm::to_string(new_y) << std::endl;
  std::cout << glm::to_string(new_z) << std::endl;

  xform = ghost->getWorldTransform();
  btVector3 forwardDir = xform.getBasis()[0];
  btVector3 upDir = xform.getBasis()[1];
  btVector3 strafeDir = xform.getBasis()[2];
  printf("x=%f,%f,%f\n",forwardDir[0],forwardDir[1],forwardDir[2]);
  printf("y=%f,%f,%f\n",upDir[0],upDir[1],upDir[2]);
  printf("z=%f,%f,%f\n",strafeDir[0],strafeDir[1],strafeDir[2]);
  ghost->getWorldTransform().getOpenGLMatrix((btScalar *) &m);

  glm::vec3 new_x2(m[0]);
  glm::vec3 new_y2(m[1]);
  glm::vec3 new_z2(m[2]);
  std::cout << glm::to_string(new_x2) << std::endl;
  std::cout << glm::to_string(new_y2) << std::endl;
  std::cout << glm::to_string(new_z2) << std::endl;

  forwardDir.normalize ();
  upDir.normalize ();
  strafeDir.normalize ();

  btScalar walkVelocity = btScalar(1.1) * 4.0; // 4 km/h -> 1.1 m/s
  //btScalar walkSpeed = walkVelocity * dt;
  btScalar walkSpeed = walkVelocity * 0.16;

  if (direction & PHYSICS_DIRECTION_LEFT) {
    /*
    btMatrix3x3 orn = ghost->getWorldTransform().getBasis();
    orn *= btMatrix3x3(btQuaternion(btVector3(0, 0, 1), 0.05));
    ghost->getWorldTransform().setBasis(orn);
*/
    node->mesh->model = glm::rotate(node->mesh->model, 0.1f, glm::vec3(0, 0, 1));
  }

  if (direction & PHYSICS_DIRECTION_RIGHT) {
    /*
    btMatrix3x3 orn = ghost->getWorldTransform().getBasis();
    orn *= btMatrix3x3(btQuaternion(btVector3(0, 0, 1), -0.05));
    ghost->getWorldTransform().setBasis(orn);
    */
    node->mesh->model = glm::rotate(node->mesh->model, -0.1f, glm::vec3(0, 0, 1));
  }

  ghost->getWorldTransform().setFromOpenGLMatrix((btScalar *) &node->mesh->model);

  glm::vec3 forward = glm::vec3(node->mesh->model[1]);
  glm::vec3 v(0.0f, 0.f, 0.f);

  if (direction & PHYSICS_DIRECTION_FORWARD) {
    node->mesh->model = glm::translate(node->mesh->model, forward);
   // v += forward;
  }

  if (direction & PHYSICS_DIRECTION_BACK) {
    //v -= forward;
  }


 // btVector3 walkDirection(v.x, v.y, v.z);
 // setWalkDirection(walkDirection * walkSpeed);
  //ghost->getWorldTransform().getOpenGLMatrix((btScalar *) &m);
  //node->mesh->model = m;
  /*
  node->mesh->model[0] = glm::vec4(forwardDir[0], forwardDir[1], forwardDir[2], 0.0);
  node->mesh->model[1] = glm::vec4(upDir[0], upDir[1], upDir[2], 0.0);
  node->mesh->model[2] = glm::vec4(strafeDir[0], strafeDir[1], strafeDir[2], 0.0);
  */
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
}


void Physics_CharacterController::node_set(Node &node)
{
  this->node = &node;
}


void Physics_CharacterController::move(Physics_Direction direction)
{
  this->direction = direction;

}

