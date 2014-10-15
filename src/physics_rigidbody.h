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
class Raycast_Hitpoint;
class Mesh;
class Node;

using glm::vec3;


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


struct Aabb {
  vec3 min;
  vec3 max;
  vec3 c;
  vec3 r;
};


struct Bounding_Sphere {
  vec3 c;
  float r;
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
    std::unique_ptr<btRigidBody> bt_rigidbody;
    std::unique_ptr<btTriangleMesh> bt_triangle_mesh;
    std::unique_ptr<btConvexHullShape> bt_convex_hull_mesh;
    std::unique_ptr<btGeneric6DofConstraint> bt_dof6;
    float mass = 0;

    Node *node_ptr = nullptr;
    Physics_Collision_Shape *shape_ptr = nullptr;
    std::unique_ptr<Physics_Motion_State> bt_motion_state;
    unsigned int type = -1;

    void bt_shape_init(Node &node, btCollisionShape *shape, unsigned int type, float inital_mass);


  public:
    std::shared_ptr<Aabb>    aabb_get();
    std::shared_ptr<Bounding_Sphere> bounding_sphere_get();
    btRigidBody             *bt_rigidbody_get();
    btGeneric6DofConstraint *bt_dof6_get();
    void                     create(Physics &physics, Physics_Collision_Shape &shape, unsigned int collision_type, float initial_mass);
    void                     constraint_create(Raycast_Hitpoint &hp);
    void                     constraint_delete();
    void                     motionstate_update(Node *node);
    void                     mass_set(Physics *physics, const float mass);
    float                    mass_get();
    Node                    *node_ptr_get();
    void                     node_ptr_set(Node *node_ptr);
    Physics_Collision_Shape *shape_get();
    unsigned int             type_get();

};

