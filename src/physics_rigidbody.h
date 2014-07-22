#pragma once
#include <btBulletDynamicsCommon.h>
#include <memory>

class Physics_Rigidbody
{
  private:
    std::unique_ptr<btCollisionShape> collision_shape;
    std::unique_ptr<btRigidBody> bullet_rb;
    float mass;


  public:
    Physics_Rigidbody();
    ~Physics_Rigidbody();

    void collision_shape_add(const unsigned int shape);
};

