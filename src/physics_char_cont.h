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


class Physics_CharacterController: public btKinematicCharacterController
{
  private:
    Node *node;
    unsigned int direction;

  public:
    float angle_joystick;
    float rotation_angle;

    using btKinematicCharacterController::btKinematicCharacterController;
    ~Physics_CharacterController();

    void joystick_angle_set(const float angle)
    {
      this->angle_joystick = angle;
      
    }

    void bullet_character_step();
    void bullet_debug_draw_contacts(btDiscreteDynamicsWorld *world, btBroadphaseInterface *broadphase);

    void defaults_set();
    void node_set(Node &node);
    void move(Physics_Direction dir);
    void move_x(const float x);
    void move_z(const float z);
    void move_halt();
    void rotate(const float x);


};

