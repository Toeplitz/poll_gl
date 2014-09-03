#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <btBulletDynamicsCommon.h>
#include <memory>


using namespace glm;
class Physics_Motion_State;


class Mesh;
class Node;


class Physics_Rigidbody
{
  private:
    std::unique_ptr<btCollisionShape> bt_collision_shape;
    std::unique_ptr<btRigidBody> bt_rigidbody;
    std::unique_ptr<btTriangleMesh> bt_triangle_mesh;
    float mass = 0;

    Node *node_ptr = nullptr;
    std::unique_ptr<Physics_Motion_State> bt_motion_state;


  public:
    enum Shape
    {
      BOX,
      SPHERE,
      CONVEX_HULL,
      TRIANGLE_MESH
    };

    btRigidBody *bt_rigidbody_get();
    void         create(Node *node, const unsigned int shape = Shape::BOX);
    void         mass_set(const float mass);
    Node        *node_ptr_get();
    void         node_ptr_set(Node *node_ptr);
    void         motionstate_transform_set(const mat4 &transform);

};

