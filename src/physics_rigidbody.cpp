#include <iostream>
#include "node.h"
#include "physics.h"
#include "physics_rigidbody.h"
#include <glm/gtx/string_cast.hpp>
#include "utils.h"


btRigidBody *Physics_Rigidbody::bt_rigidbody_get()
{
  return bt_rigidbody.get();
}


void Physics_Rigidbody::create(Node *node_ptr, unsigned int shape, unsigned int type)
{
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
      POLL_DEBUG(std::cout, "box");
      break;
    case SPHERE:
      bt_collision_shape = std::unique_ptr<btSphereShape>(new btSphereShape(btScalar(1.f)));
      break;
    case CONVEX_HULL:
      {
        Mesh *mesh = node_ptr->mesh_get();
        std::vector<glm::vec3> positions = mesh->positions_get();
       // bt_convex_hull_mesh = std::unique_ptr<btConvexHullShape>(new btConvexHullShape());

        bt_collision_shape = std::unique_ptr<btConvexShape>(new btConvexHullShape((btScalar *) positions.data(), 
              mesh->num_vertices_get(), sizeof(glm::vec3)));
       // bt_collision_shape = std::unique_ptr<btConvexShape>(bt_convex_hull_mesh.get());

        /*

        for (unsigned int i = 0; i < mesh->num_vertices_get(); i++) {
          bt_convex_hull_mesh->addPoint(btVector3(positions[i].x , positions[i].y, positions[i].z));
        }
        */

      }

      break;
    case TRIANGLE_MESH:
      {
        Mesh *mesh = node_ptr->mesh_get();
        int num_indices = mesh->num_indices_get();

        std::vector<glm::vec3> positions = mesh->positions_get();
        std::vector<GLshort> indices = mesh->indices_get();

        assert(node.mesh_get()->num_indices_get() % 3 == 0);

        bt_triangle_mesh = std::unique_ptr<btTriangleMesh> (new btTriangleMesh());

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
        bt_collision_shape->setUserPointer(node_ptr);

      }
      break;
    default:
      std::cout << "Error: shape not supported" << std::endl;
      return;
  }

  btTransform t;
  t.setFromOpenGLMatrix((btScalar *) &node_ptr->transform_global_get());

  bt_motion_state = std::unique_ptr<Physics_Motion_State>(new Physics_Motion_State(t, *node_ptr));

  btVector3 inertia(0, 0, 0);
  btScalar bt_mass = mass;
  bt_collision_shape->calculateLocalInertia(mass, inertia);
  btRigidBody::btRigidBodyConstructionInfo rb_ci(bt_mass, bt_motion_state.get(), bt_collision_shape.get(), inertia);
  
  bt_rigidbody = std::unique_ptr<btRigidBody>(new btRigidBody(rb_ci));

  if (type == Type::KINEMATIC) {
    bt_rigidbody->setCollisionFlags(bt_rigidbody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT); 
    bt_rigidbody->setActivationState(DISABLE_DEACTIVATION);
  }
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


void Physics_Rigidbody::motionstate_transform_set(const mat4 &transform)
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
