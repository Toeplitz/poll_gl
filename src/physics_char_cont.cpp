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

#include <iostream>


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

//  std::cout << glm::to_string(walking_dir) << std::endl;

  setWalkDirection(btVector3(walking_dir.x, walking_dir.y, walking_dir.z));
  ghost->getWorldTransform().getOpenGLMatrix((btScalar *) &m);
  node->mesh->model = m;
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
  walking_dir = glm::vec3(0.0f, 0.0f, 0.0f);
  max_step = 0.25f;
}


void Physics_CharacterController::node_set(Node &node)
{
  this->node = &node;
}


void Physics_CharacterController::move(glm::fvec3 &v)
{
  glm::vec3 check = walking_dir + v;
  std::cout << glm::to_string(check) << std::endl;

  std::cout << "abs: "<<  fabs(check.z) << " max_step: " << max_step << std::endl;

  if (fabs(check.z) > max_step) {
    std::cout << max_step << std::endl;
    std::cout << "returning" << std::endl;
    return;
  }

  walking_dir += v;
}




