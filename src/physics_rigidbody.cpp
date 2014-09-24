#include <iostream>
#include "node.h"
#include "physics.h"
#include "physics_rigidbody.h"
#include <glm/gtx/string_cast.hpp>
#include "utils.h"


/**************************************************/
/********** PHYSICS COLLISION SHAPE CLASS *********/
/**************************************************/
/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/


void  Physics_Collision_Shape::bt_collision_shape_add(std::unique_ptr<btCollisionShape> &&shape)
{
  bt_collision_shape = std::move(shape);
}


btCollisionShape &Physics_Collision_Shape::bt_collision_shape_get()
{
  return *bt_collision_shape;
}


void Physics_Collision_Shape::bt_mesh_populate(Node &node, btTriangleMesh &bt_triangle_mesh)
{
  Mesh &mesh = *node.mesh_get();
  int num_indices = mesh.num_indices_get();
  std::vector<glm::vec3> positions = mesh.positions_get();
  std::vector<GLshort> indices = mesh.indices_get();

  if (num_indices > 0) { 
    for (unsigned int i = 0; i < mesh.num_indices_get(); i = i + 3) {
      bt_triangle_mesh.addTriangle(btVector3(positions[indices[i]].x, positions[indices[i]].y, positions[indices[i]].z),
          btVector3(positions[indices[i + 1]].x, positions[indices[i + 1]].y, positions[indices[i + 1]].z),
          btVector3(positions[indices[i + 2]].x, positions[indices[i + 2]].y, positions[indices[i + 2]].z));
    }
  } else {
    for (unsigned int i = 0; i < mesh.num_vertices_get(); i = (i + 3)) {
      vec3 pos_1 = positions[i];
      vec3 pos_2 = positions[i + 1];
      vec3 pos_3 = positions[i + 2];
      bt_triangle_mesh.addTriangle(btVector3(pos_1.x, pos_1.y, pos_1.z),
          btVector3(pos_2.z, pos_2.y, pos_2.z),
          btVector3(pos_3.z, pos_3.y, pos_3.z));
    }
  }

}


Physics_Box_Shape::Physics_Box_Shape(glm::vec3 &v)
{
  auto shape = std::unique_ptr<btBoxShape>(new btBoxShape(btVector3(v.x, v.y, v.z)));
  bt_collision_shape_add(std::move(shape));
}


Physics_Convex_Hull_Shape::Physics_Convex_Hull_Shape(Node &node)
{
  bt_triangle_mesh = std::unique_ptr<btTriangleMesh>(new btTriangleMesh());
  bt_mesh_populate(node, *bt_triangle_mesh);
  auto shape = std::unique_ptr<btConvexTriangleMeshShape>(new btConvexTriangleMeshShape(bt_triangle_mesh.get()));
  bt_collision_shape_add(std::move(shape));
}


Physics_Triangle_Mesh_Shape::Physics_Triangle_Mesh_Shape(Node &node)
{
  bt_triangle_mesh = std::unique_ptr<btTriangleMesh>(new btTriangleMesh());
  bt_mesh_populate(node, *bt_triangle_mesh);
  btVector3 aabb_min(-1000, -1000, -1000), aabb_max(1000, 1000, 1000);
  auto shape = std::unique_ptr<btBvhTriangleMeshShape>(new btBvhTriangleMeshShape(bt_triangle_mesh.get(), true, aabb_min, aabb_max));
  bt_collision_shape_add(std::move(shape));
}


/**************************************************/
/************* PHYSICS RIGIDBODY CLASS ************/
/**************************************************/
/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/


btRigidBody *Physics_Rigidbody::bt_rigidbody_get()
{
  return bt_rigidbody.get();
}


void Physics_Rigidbody::create(Node *node_ptr, Physics_Collision_Shape *shape, unsigned int collision_type, float initial_mass)
{
  bt_shape_init(node_ptr, &shape->bt_collision_shape_get(), collision_type, initial_mass);
}


void Physics_Rigidbody::create(Node *node_ptr, unsigned int shape, unsigned int type, float initial_mass)
{
  btCollisionShape *shape_ptr;

  if (!node_ptr) {
    std::cout << "Error: no mesh, cannot create a rigidbody" << std::endl;
    return;
  }

  this->node_ptr = node_ptr;
  this->shape = shape;
  this->type = type;

  switch (shape) {
    case BOX:
      bt_collision_shape = std::unique_ptr<btBoxShape>(new btBoxShape(btVector3(1.f, 1.f, 1.f)));
      shape_ptr = bt_collision_shape.get();
      break;
    case SPHERE:
      bt_collision_shape = std::unique_ptr<btSphereShape>(new btSphereShape(btScalar(1.f)));
      shape_ptr = bt_collision_shape.get();
      break;
    case CONVEX_HULL:
      {
        Mesh *mesh = node_ptr->mesh_get();
        int num_indices = mesh->num_indices_get();

        std::vector<glm::vec3> positions = mesh->positions_get();
        std::vector<GLshort> indices = mesh->indices_get();

        bt_triangle_mesh = std::unique_ptr<btTriangleMesh>(new btTriangleMesh());

        if (num_indices > 0) { 
          for (unsigned int i = 0; i < mesh->num_indices_get(); i = i + 3) {
            bt_triangle_mesh->addTriangle(btVector3(positions[indices[i]].x, positions[indices[i]].y, positions[indices[i]].z),
                btVector3(positions[indices[i + 1]].x, positions[indices[i + 1]].y, positions[indices[i + 1]].z),
                btVector3(positions[indices[i + 2]].x, positions[indices[i + 2]].y, positions[indices[i + 2]].z));
          }
        } else {
          for (unsigned int i = 0; i < mesh->num_vertices_get(); i = (i + 3)) {
            vec3 pos_1 = positions[i];
            vec3 pos_2 = positions[i + 1];
            vec3 pos_3 = positions[i + 2];
            bt_triangle_mesh->addTriangle(btVector3(pos_1.x, pos_1.y, pos_1.z),
                btVector3(pos_2.z, pos_2.y, pos_2.z),
                btVector3(pos_3.z, pos_3.y, pos_3.z));
          }
        }
        btConvexShape *tmpshape = new btConvexTriangleMeshShape(bt_triangle_mesh.get());
        shape_ptr = tmpshape;
      }

      break;
    case TRIANGLE_MESH:
      {
        Mesh *mesh = node_ptr->mesh_get();
        int num_indices = mesh->num_indices_get();

        std::vector<glm::vec3> positions = mesh->positions_get();
        std::vector<GLshort> indices = mesh->indices_get();

        bt_triangle_mesh = std::unique_ptr<btTriangleMesh>(new btTriangleMesh());

        if (num_indices > 0) { 
          for (unsigned int i = 0; i < mesh->num_indices_get(); i = i + 3) {
            bt_triangle_mesh->addTriangle(btVector3(positions[indices[i]].x, positions[indices[i]].y, positions[indices[i]].z),
                btVector3(positions[indices[i + 1]].x, positions[indices[i + 1]].y, positions[indices[i + 1]].z),
                btVector3(positions[indices[i + 2]].x, positions[indices[i + 2]].y, positions[indices[i + 2]].z));
          }
        } else {
          for (unsigned int i = 0; i < mesh->num_vertices_get(); i = i + 3) {
            vec3 pos_1 = positions[i];
            vec3 pos_2 = positions[i + 1];
            vec3 pos_3 = positions[i + 2];
            bt_triangle_mesh->addTriangle(btVector3(pos_1.x, pos_1.y, pos_1.z),
                btVector3(pos_2.z, pos_2.y, pos_2.z),
                btVector3(pos_3.z, pos_3.y, pos_3.z));
          }
        }

        btVector3 aabbMin(-1000, -1000, -1000), aabbMax(1000, 1000, 1000);
        bt_collision_shape = std::unique_ptr<btBvhTriangleMeshShape>(new btBvhTriangleMeshShape(bt_triangle_mesh.get(), true, aabbMin, aabbMax));
        shape_ptr = bt_collision_shape.get();

      }
      break;
    default:
      std::cout << "Error: shape not supported" << std::endl;
      return;
  }

  bt_shape_init(node_ptr, shape_ptr, type, initial_mass);

}


void Physics_Rigidbody::mass_set(Physics *physics, const float mass)
{
  if (!bt_rigidbody) {
    POLL_ERROR(std::cerr, "No rigidbody existing when trying to set mass");
    return;
  }

  physics->rigidbody_delete(this);
  this->mass = mass;

  bt_rigidbody.release();
  btVector3 inertia(0, 0, 0);
  btRigidBody::btRigidBodyConstructionInfo rb_ci(mass, bt_motion_state.get(), bt_collision_shape.get(), inertia);

  bt_rigidbody = std::unique_ptr<btRigidBody>(new btRigidBody(rb_ci));
  //bt_rigidbody->getCollisionShape()->calculateLocalInertia(bt_mass, inertia);
  //bt_rigidbody->setMassProps(bt_mass, inertia);
  physics->rigidbody_add(this);
}


float Physics_Rigidbody::mass_get()
{
  return mass;
}


Node *Physics_Rigidbody::node_ptr_get()
{
  return node_ptr;
}


void Physics_Rigidbody::node_ptr_set(Node *node_ptr)
{
  this->node_ptr = node_ptr;
  bt_motion_state->node_set(*node_ptr);
}


void Physics_Rigidbody::motionstate_transform_set(glm::mat4 &transform)
{
  bt_motion_state->transform_set(transform);
}


unsigned int Physics_Rigidbody::shape_get()
{
  return shape;
}


unsigned int Physics_Rigidbody::type_get()
{
  return type;
}


void Physics_Rigidbody::bt_shape_init(Node *node_ptr, btCollisionShape *shape, unsigned int type, float initial_mass)
{
  bt_motion_state = std::unique_ptr<Physics_Motion_State>(new Physics_Motion_State(*node_ptr));

  btVector3 inertia(0, 0, 0);
  btScalar bt_mass = initial_mass;
  shape->calculateLocalInertia(bt_mass, inertia);
  btRigidBody::btRigidBodyConstructionInfo rb_ci(bt_mass, bt_motion_state.get(), shape, inertia);
  this->mass = initial_mass;

  bt_rigidbody = std::unique_ptr<btRigidBody>(new btRigidBody(rb_ci));
  bt_rigidbody->setUserPointer((void*) node_ptr);

  if (type == Type::KINEMATIC) {
    bt_rigidbody->setCollisionFlags(bt_rigidbody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT); 
    bt_rigidbody->setActivationState(DISABLE_DEACTIVATION);
  }


}
