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

  btTransform xform;

  /*
  glm::vec3 new_x(node->mesh->model[0]);
  glm::vec3 new_y(node->mesh->model[1]);
  glm::vec3 new_z(node->mesh->model[2]);
  std::cout << glm::to_string(new_x) << std::endl;
  std::cout << glm::to_string(new_y) << std::endl;
  std::cout << glm::to_string(new_z) << std::endl;
*/

  xform = ghost->getWorldTransform();
  btVector3 forwardDir = xform.getBasis()[2];
  btVector3 upDir = xform.getBasis()[1];
  btVector3 strafeDir = xform.getBasis()[0];
  /*
  printf("x=%f,%f,%f\n",forwardDir[0],forwardDir[1],forwardDir[2]);
  printf("y=%f,%f,%f\n",upDir[0],upDir[1],upDir[2]);
  printf("z=%f,%f,%f\n",strafeDir[0],strafeDir[1],strafeDir[2]);

  glm::vec3 new_x2(m[0]);
  glm::vec3 new_y2(m[1]);
  glm::vec3 new_z2(m[2]);
  std::cout << glm::to_string(new_x2) << std::endl;
  std::cout << glm::to_string(new_y2) << std::endl;
  std::cout << glm::to_string(new_z2) << std::endl;
*/

  forwardDir.normalize ();
  upDir.normalize ();
  strafeDir.normalize ();

  //btVector3 walkDirection = btVector3(0.0, 0.0, 0.0);
  btScalar walkVelocity = btScalar(1.1) * 3.0; // 4 km/h -> 1.1 m/s
 // btScalar walkSpeed = walkVelocity * dt;
  btScalar walkSpeed = walkVelocity * 0.16;

  if (direction & PHYSICS_DIRECTION_LEFT) {
    /*
    btMatrix3x3 orn = ghost->getWorldTransform().getBasis();
    orn *= btMatrix3x3(btQuaternion(btVector3(0, 0, 1), 0.05));
    ghost->getWorldTransform().setBasis(orn);
*/
    std::cout << "Physics move left" << std::endl;
    node->mesh->model = glm::rotate(node->mesh->model, 0.1f, glm::vec3(0, 1, 0));
  }

  if (direction & PHYSICS_DIRECTION_RIGHT) {
    /*
    btMatrix3x3 orn = ghost->getWorldTransform().getBasis();
    orn *= btMatrix3x3(btQuaternion(btVector3(0, 0, 1), -0.05));
    ghost->getWorldTransform().setBasis(orn);
    */
    std::cout << "Physics move right" << std::endl;
    node->mesh->model = glm::rotate(node->mesh->model, -0.1f, glm::vec3(0, 1, 0));
  }


  glm::vec3 forward = glm::vec3(node->mesh->model[2]);
  print_matrix(std::cout, node->transform_global, 0);
  std::cout << glm::to_string(node->original_position) << std::endl;
  print_matrix(std::cout, node->mesh->model, 0);
  glm::vec3 v(0.f, 0.f, 0.f);

  if (direction & PHYSICS_DIRECTION_FORWARD) {
    v += forward;
 //   walkDirection += forwardDir;
  }

  if (direction & PHYSICS_DIRECTION_BACK) {
    v -= forward;
//    walkDirection -= forwardDir;
  }
  glm::normalize(v);


  btVector3 walkDirection(v.x, v.y, v.z);
  std::cout << walkDirection.x() << ", " << walkDirection.y() << ", " << walkDirection.z() << std::endl;
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
}


void Physics_CharacterController::node_set(Node &node)
{
  this->node = &node;
}


void Physics_CharacterController::move(Physics_Direction direction)
{
  this->direction = direction;

}

