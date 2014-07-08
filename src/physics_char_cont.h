#pragma once

#include <btBulletDynamicsCommon.h>
#include <LinearMath/btVector3.h>
#include <BulletDynamics/Character/btCharacterControllerInterface.h>
#include <BulletCollision/BroadphaseCollision/btCollisionAlgorithm.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include "node.h"



class Physics_CharacterController: public btKinematicCharacterController
{
  private:
    glm::vec3 walking_dir;
    float rotation;
    float max_step;
    Node *node;

  public:
    using btKinematicCharacterController::btKinematicCharacterController;
    ~Physics_CharacterController();

    void bullet_character_step();
    void bullet_debug_draw_contacts(btDiscreteDynamicsWorld *world, btBroadphaseInterface *broadphase);

    void defaults_set();
    void node_set(Node &node);
    void move(glm::fvec3 &v);

};

