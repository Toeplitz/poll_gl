#include <iostream>
#include "node.h"
#include "physics.h"
#include "physics_rigidbody.h"
#include "raycast.h"
#include <glm/gtx/string_cast.hpp>
#include "utils.h"
#include <memory>


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
  Mesh *mesh = node.mesh_get();

  if (!mesh) {
    POLL_ERROR(std::cerr, "No mesh for node: " << node.name_get());
    return;
  }

  int num_indices = mesh->num_indices_get();
  std::vector<glm::vec3> positions = mesh->positions_get();
  std::vector<GLshort> indices = mesh->indices_get();

  if (num_indices > 0) { 
    for (unsigned int i = 0; i < mesh->num_indices_get(); i = i + 3) {
      bt_triangle_mesh.addTriangle(btVector3(positions[indices[i]].x, positions[indices[i]].y, positions[indices[i]].z),
          btVector3(positions[indices[i + 1]].x, positions[indices[i + 1]].y, positions[indices[i + 1]].z),
          btVector3(positions[indices[i + 2]].x, positions[indices[i + 2]].y, positions[indices[i + 2]].z));
    }
  } else {
    for (unsigned int i = 0; i < mesh->num_vertices_get(); i = (i + 3)) {
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
  auto shape = std::unique_ptr<btConvexTriangleMeshShape>(new btConvexTriangleMeshShape(bt_triangle_mesh.get(), false));
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


std::shared_ptr<Aabb> Physics_Rigidbody::aabb_get(bool switch_yz)
{
  auto aabb = std::shared_ptr<Aabb>(new Aabb());
  btVector3 aabb_min;
  btVector3 aabb_max;

  bt_rigidbody->getAabb(aabb_min, aabb_max);

  if (switch_yz) {
    aabb->min = vec3(aabb_min.getX(), aabb_min.getY(), aabb_min.getZ());
    aabb->max = vec3(aabb_max.getX(), aabb_max.getY(), aabb_max.getZ());
  } else {
    aabb->min = vec3(aabb_min.getX(), aabb_min.getZ(), aabb_min.getY());
    aabb->max = vec3(aabb_max.getX(), aabb_max.getZ(), aabb_max.getY());
  }

  aabb->r = (aabb->max - aabb->min) / 2.f;
  aabb->c = aabb->min + aabb->r;

  return aabb;
}


std::shared_ptr<Bounding_Sphere> Physics_Rigidbody::bounding_sphere_get()
{
  auto bs = std::shared_ptr<Bounding_Sphere>(new Bounding_Sphere());

  btVector3 center;
  btVector3 local_scaling;
  btScalar radius;

  if (!shape_ptr) {
    POLL_DEBUG(std::cerr, "No shape connected to the rigidbody, not possible??");
  }

  shape_ptr->bt_collision_shape_get().getBoundingSphere(center, radius);
  local_scaling = shape_ptr->bt_collision_shape_get().getLocalScaling();

  bs->r = (float) radius;
  bs->c = vec3(center.getX(), center.getY(), center.getZ());
  bs->local_scaling = vec3(local_scaling.getX(), local_scaling.getY(), local_scaling.getZ());

  return bs;
}

btRigidBody *Physics_Rigidbody::bt_rigidbody_get()
{
  return bt_rigidbody.get();
}


btGeneric6DofConstraint *Physics_Rigidbody::bt_dof6_get()
{
  return bt_dof6.get();
}


void Physics_Rigidbody::constraint_create(Raycast_Hitpoint &hp)
{
  bt_rigidbody->setActivationState(DISABLE_DEACTIVATION);
  btVector3 pickPos = btVector3(hp.world_hitpoint.x, hp.world_hitpoint.y, hp.world_hitpoint.z);
  btVector3 localPivot = bt_rigidbody->getCenterOfMassTransform().inverse() * pickPos;

  printf("pickPos=%f,%f,%f\n",pickPos.getX(),pickPos.getY(),pickPos.getZ());
  printf("localPivot=%f,%f,%f\n",localPivot.getX(),localPivot.getY(),localPivot.getZ());

  btTransform tr;
  tr.setIdentity();
  tr.setOrigin(localPivot);
  bt_dof6 = std::unique_ptr<btGeneric6DofConstraint>(new btGeneric6DofConstraint(*bt_rigidbody, tr, true));
  bt_dof6->setLinearLowerLimit(btVector3(0, 0, 0));
  bt_dof6->setLinearUpperLimit(btVector3(0, 0, 0));
  bt_dof6->setAngularLowerLimit(btVector3(0, 0, 0));
  bt_dof6->setAngularUpperLimit(btVector3(0, 0, 0));

  float cfm = 0.9;
  bt_dof6->setParam(BT_CONSTRAINT_STOP_CFM, cfm, 0);
  bt_dof6->setParam(BT_CONSTRAINT_STOP_CFM, cfm, 1);
  bt_dof6->setParam(BT_CONSTRAINT_STOP_CFM, cfm, 2);
  bt_dof6->setParam(BT_CONSTRAINT_STOP_CFM, cfm, 3);
  bt_dof6->setParam(BT_CONSTRAINT_STOP_CFM, cfm, 4);
  bt_dof6->setParam(BT_CONSTRAINT_STOP_CFM, cfm, 5);

  float erp = 1.0;
  bt_dof6->setParam(BT_CONSTRAINT_STOP_ERP, erp, 0);
  bt_dof6->setParam(BT_CONSTRAINT_STOP_ERP, erp, 1);
  bt_dof6->setParam(BT_CONSTRAINT_STOP_ERP, erp, 2);
  bt_dof6->setParam(BT_CONSTRAINT_STOP_ERP, erp, 3);
  bt_dof6->setParam(BT_CONSTRAINT_STOP_ERP, erp, 4);
  bt_dof6->setParam(BT_CONSTRAINT_STOP_ERP, erp, 5);

}


void Physics_Rigidbody::constraint_delete()
{
  bt_rigidbody->forceActivationState(ACTIVE_TAG);
  bt_rigidbody->setDeactivationTime(0.f);
}


void Physics_Rigidbody::create(Physics &physics, Physics_Collision_Shape &shape, unsigned int collision_type, float initial_mass)
{
  this->shape_ptr = &shape;
  this->type = collision_type;
  bt_shape_init(*node_ptr_get(), &shape.bt_collision_shape_get(), collision_type, initial_mass);
  physics.rigidbody_add(this);
}

  
void  Physics_Rigidbody::filter_group_add(const Physics_Collision_Filter &filter)
{
  bt_rigidbody_get()->getBroadphaseProxy()->m_collisionFilterGroup |= filter;
}


void  Physics_Rigidbody::filter_group_remove(const Physics_Collision_Filter &filter)
{
  bt_rigidbody_get()->getBroadphaseProxy()->m_collisionFilterGroup &= ~filter;
}


void Physics_Rigidbody::filter_group_raycast_toggle()
{
  if (bt_rigidbody_get()->getBroadphaseProxy()->m_collisionFilterGroup & PHYSICS_RAYCAST_ENABLED) {
    filter_group_remove(PHYSICS_RAYCAST_ENABLED);
  } else {
    filter_group_add(PHYSICS_RAYCAST_ENABLED);
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
 // btRigidBody::btRigidBodyConstructionInfo rb_ci(mass, bt_motion_state.get(), bt_collision_shape.get(), inertia);

 // bt_rigidbody = std::unique_ptr<btRigidBody>(new btRigidBody(rb_ci));
  //bt_rigidbody->getCollisionShape()->calculateLocalInertia(bt_mass, inertia);
  //bt_rigidbody->setMassProps(bt_mass, inertia);
  //physics->rigidbody_add(this);
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
}


void Physics_Rigidbody::motionstate_update(Node *node)
{
  if (!node) {
    POLL_ERROR(std::cerr, "no node linked to the motionstate");
    return;
  }

  bt_motion_state->node_set(*node);
}


Physics_Collision_Shape *Physics_Rigidbody::shape_get()
{
  if (!shape_ptr) {
    POLL_ERROR(std::cerr, "no collision shape poiner for rigidbody");

    if (!node_ptr_get()) {
      POLL_DEBUG(std::cout, "name: " << node_ptr_get()->name_get());
    } else {
      POLL_ERROR(std::cerr, "no node pointer set for rigidbody");
    }
  }
  return shape_ptr;
}


unsigned int Physics_Rigidbody::type_get()
{
  return type;
}


void Physics_Rigidbody::bt_shape_init(Node &node, btCollisionShape *shape, unsigned int type, float initial_mass)
{
  bt_motion_state = std::unique_ptr<Physics_Motion_State>(new Physics_Motion_State(node));
  bt_motion_state->node_set(node);

  btVector3 inertia(0, 0, 0);
  btScalar bt_mass = initial_mass;
  shape->calculateLocalInertia(bt_mass, inertia);
  btRigidBody::btRigidBodyConstructionInfo rb_ci(bt_mass, bt_motion_state.get(), shape, inertia);
  this->mass = initial_mass;


  bt_rigidbody = std::unique_ptr<btRigidBody>(new btRigidBody(rb_ci));
  bt_rigidbody->setUserPointer((void*) &node);

  if (type == Type::KINEMATIC) {
    bt_rigidbody->setCollisionFlags(bt_rigidbody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT); 
    bt_rigidbody->setActivationState(DISABLE_DEACTIVATION);
  }

}
