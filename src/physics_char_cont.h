#pragma once

#include <btBulletDynamicsCommon.h>
#include <LinearMath/btVector3.h>
#include <BulletDynamics/Character/btCharacterControllerInterface.h>
#include <BulletCollision/BroadphaseCollision/btCollisionAlgorithm.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include "node.h"


enum Physics_Direction
{
  PHYSICS_DIRECTION_FORWARD  = 1 << 0,
  PHYSICS_DIRECTION_BACK = 1 << 1,
  PHYSICS_DIRECTION_LEFT = 1 << 2,
  PHYSICS_DIRECTION_RIGHT = 1 << 3,
  PHYSICS_DIRECTION_STRAFE_LEFT  = 1 << 4,
  PHYSICS_DIRECTION_STRAFE_RIGHT = 1 << 5,
  PHYSICS_DIRECTION_ROTATE = 1 << 6,
  PHYSICS_DIRECTION_NONE= 1 << 7 
};


enum Physics_Character_State
{
  CHARACTER_STATE_IDLE = 1 << 0,
  CHARACTER_STATE_MOVING = 1 << 1,
  CHARACTER_STATE_JUMPING = 1 << 2
};


class Physics_Character_Controller: public btKinematicCharacterController
{
  private:
    Node *node;
    unsigned int direction;
    Physics_Character_State state;

  public:
    float angle_joystick;
    float rotation_angle;

    using btKinematicCharacterController::btKinematicCharacterController;
    ~Physics_Character_Controller();


    void bullet_character_step(const double dt);
    void bullet_debug_draw_contacts(btDiscreteDynamicsWorld *world, btBroadphaseInterface *broadphase);

    void                           joystick_angle_set(const float angle);
    void                           reset();
    void                           node_set(Node &node);
    void                           move(Physics_Direction dir);
    const Physics_Character_State  state_get();
    void                           rotate(const float x);


};

