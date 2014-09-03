#include <iostream>
#include "node.h"
#include "physics.h"
#include "physics_rigidbody.h"
#include <glm/gtx/string_cast.hpp>


btRigidBody *Physics_Rigidbody::bt_rigidbody_get()
{
  return bt_rigidbody.get();
}


void Physics_Rigidbody::create(Node *node_ptr, const unsigned int shape)
{
  if (!node_ptr) {
    std::cout << "Error: no mesh, cannot create a rigidbody" << std::endl;
    return;
  }

  this->node_ptr = node_ptr;
  glm::vec3 position = node_ptr->original_position_get();
  glm::vec3 scaling = node_ptr->original_scaling_get();
  glm::quat rotation = node_ptr->original_rotation_get();

  switch (shape) {
    case BOX:
      bt_collision_shape = std::unique_ptr<btBoxShape>(new btBoxShape(btVector3(scaling.x, scaling.y, scaling.z)));
      break;
    case SPHERE:
      bt_collision_shape = std::unique_ptr<btSphereShape>(new btSphereShape(btScalar(scaling.x)));
      break;
    case CONVEX_HULL:
      {
        std::vector<glm::vec3> vertices = node_ptr->mesh_get()->positions_get();
        int n = vertices.size();

        bt_collision_shape = std::unique_ptr<btConvexShape>(new btConvexHullShape((btScalar *) vertices.data(), n, sizeof(glm::vec3)));
        bt_collision_shape->setLocalScaling(btVector3(node_ptr->original_scaling_get().x, 
              node_ptr->original_scaling_get().y, node_ptr->original_scaling_get().z));
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

            /*
            std::cout << glm::to_string(positions[indices[i]]) << std::endl;
            std::cout << glm::to_string(positions[indices[i + 1]]) << std::endl;
            std::cout << glm::to_string(positions[indices[i + 2]]) << std::endl;
            */

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

        /*
        bt_collision_shape->setLocalScaling(btVector3(node_ptr->original_scaling_get().x, 
              node_ptr->original_scaling_get().y, node_ptr->original_scaling_get().z));
              */
      }
      break;
    default:
      std::cout << "Error: shape not supported" << std::endl;
      return;
  }

  glm::mat4 matrix;
  btTransform t;
  t.setIdentity();
  t.setOrigin(btVector3(position.x, position.y, position.z));
  t.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w));
  btTransform t2;
  t2.setFromOpenGLMatrix((btScalar *) &node_ptr->transform_model_get());

 // std::cout << to_string(node_ptr->transform_model_get()) << std::endl;

  bt_motion_state = std::unique_ptr<Physics_Motion_State>(new Physics_Motion_State(t2, *node_ptr));

  btVector3 inertia(0, 0, 0);
  btScalar bt_mass = mass;
  bt_collision_shape->calculateLocalInertia(mass, inertia);
  btRigidBody::btRigidBodyConstructionInfo rb_ci(bt_mass, bt_motion_state.get(), bt_collision_shape.get(), inertia);
  
  bt_rigidbody = std::unique_ptr<btRigidBody>(new btRigidBody(rb_ci));

  bt_rigidbody->setCollisionFlags(bt_rigidbody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT); 
  bt_rigidbody->setActivationState(DISABLE_DEACTIVATION);
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


