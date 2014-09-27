#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "gldefaults.h"
#include <btBulletDynamicsCommon.h>
#include <memory>
#include <vector>


class Physics_Motion_State;
class Physics;


class Mesh;
class Node;

class Physics_Collision_Shape
{
  private:
    std::unique_ptr<btCollisionShape> bt_collision_shape;

  public:
    void              bt_collision_shape_add(std::unique_ptr<btCollisionShape> &&shape);
    btCollisionShape &bt_collision_shape_get();
    void              bt_mesh_populate(Node &node, btTriangleMesh &bt_triangle_mesh);
};


class Physics_Box_Shape: public Physics_Collision_Shape
{
  public:
    Physics_Box_Shape(glm::vec3 &v);

};


class Physics_Convex_Hull_Shape: public Physics_Collision_Shape
{
  private:
    std::unique_ptr<btTriangleMesh> bt_triangle_mesh;

  public:
    Physics_Convex_Hull_Shape(Node &node);

};


class Physics_Triangle_Mesh_Shape: public Physics_Collision_Shape
{
  private:
    std::unique_ptr<btTriangleMesh> bt_triangle_mesh;

  public:
    Physics_Triangle_Mesh_Shape(Node &node);

};

class Physics_Rigidbody
{
  public:
    enum Type
    {
      DYNAMIC = 0,
      KINEMATIC = 1
    };


  private:
    std::unique_ptr<btCollisionShape> bt_collision_shape;
    std::unique_ptr<btRigidBody> bt_rigidbody;
    std::unique_ptr<btTriangleMesh> bt_triangle_mesh;
    std::unique_ptr<btConvexHullShape> bt_convex_hull_mesh;
    float mass = 0;

    Node *node_ptr = nullptr;
    Physics_Collision_Shape *shape_ptr = nullptr;
    std::unique_ptr<Physics_Motion_State> bt_motion_state;
    unsigned int type = -1;

    void bt_shape_init(Node &node, btCollisionShape *shape, unsigned int type, float inital_mass);


  public:

    btRigidBody *bt_rigidbody_get();
    void         create(Physics &physics, Node &node, Physics_Collision_Shape &shape, unsigned int collision_type, float initial_mass);
    void         motionstate_update(Node &node);
    void         mass_set(Physics *physics, const float mass);
    float        mass_get();
    Node        *node_ptr_get();
    void         node_ptr_set(Node *node_ptr);
    Physics_Collision_Shape *shape_get();
    unsigned int type_get();

};

