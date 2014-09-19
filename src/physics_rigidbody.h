#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "gldefaults.h"
#include <btBulletDynamicsCommon.h>
#include <memory>
#include <vector>


using namespace glm;
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
    void              bt_mesh_populate(Mesh &mesh, btTriangleMesh &bt_triangle_mesh);
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
    Physics_Convex_Hull_Shape(std::vector<GLshort> &indices, std::vector<glm::vec3> &positions);

};


class Physics_Triangle_Mesh_Shape: public Physics_Collision_Shape
{
  public:
    Physics_Triangle_Mesh_Shape(std::vector<GLshort> &indices, std::vector<glm::vec3> &positions);

};

class Physics_Rigidbody
{
  public:
    enum Shape
    {
      BOX = 0,
      SPHERE = 1,
      CONVEX_HULL = 2,
      TRIANGLE_MESH = 3
    };

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
    std::unique_ptr<Physics_Motion_State> bt_motion_state;
    unsigned int shape = -1;
    unsigned int type = -1;

    void bt_shape_init(Node *node_ptr, btCollisionShape *shape, unsigned int type, float inital_mass);


  public:

    btRigidBody *bt_rigidbody_get();
    void         create(Node *node_ptr, Physics_Collision_Shape *shape);
    void         create(Node *node_ptr, unsigned int shape, unsigned int type, float initial_mass);
    void         motionstate_transform_set(const mat4 &transform);
    void         mass_set(Physics *physics, const float mass);
    float        mass_get();
    Node        *node_ptr_get();
    void         node_ptr_set(Node *node_ptr);
    unsigned int shape_get();
    unsigned int type_get();

};

