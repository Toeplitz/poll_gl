#include "node.h"
#include "physics.h"
#include "physics_char_cont.h"
#include "physics_rigidbody.h"
#include "raycast.h"
#include "scene.h"
#include "utils.h"
#include <glm/gtx/string_cast.hpp>



#if 0
struct FilterCallback : public btOverlapFilterCallback {

  // return true when pairs need collision
  virtual bool needBroadphaseCollision(btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1) const override
  {
    bool collides = (proxy0->m_collisionFilterGroup & proxy1->m_collisionFilterMask) &&
      (proxy1->m_collisionFilterGroup & proxy0->m_collisionFilterMask);
    //add some additional logic here that modified 'collides'
    return collides;
  }
};
#endif


Physics_Custom_AllHitsRayResultCallback::Physics_Custom_AllHitsRayResultCallback(const btVector3 &rayFromWorld, const btVector3 &rayToWorld):
  btCollisionWorld::AllHitsRayResultCallback(rayFromWorld, rayToWorld)
{

}


bool Physics_Custom_AllHitsRayResultCallback::needsCollision(btBroadphaseProxy *proxy0) const
{
  POLL_DEBUG(std::cout, "custom ray result callback called");

  if (proxy0->m_collisionFilterGroup & PHYSICS_RAYCAST_ENABLED) {
    POLL_DEBUG(std::cout, "Raycast is enabled for this rigidbody");
    return true;
  }

  return false;
}


/**************************************************/
/***************** CONSTRUCTORS *******************/
/**************************************************/


Physics::Physics():
  debug_toggle(false),
  pause_toggle(true)
{
  world = nullptr;
  dispatcher = nullptr;
  collision_config = nullptr;
  solver = nullptr;
  broadphase = nullptr;
  sweep_bp = nullptr;
  bullet_init();
}


Physics::~Physics()
{
  bullet_term();
}


/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/

Physics_Character_Controller *Physics::character_controller_add(Node &node, Node &collision_node)
{
  if (!node.mesh_get()) {
    std::cout << "Error: no mesh for character controller creation (node: '" << node.name_get() << "')" << std::endl;
    return nullptr;
  }

  return bullet_kinematic_character_controller_create(node, collision_node);
}


Physics_Character_Controller_List const  &Physics::character_get_all() const
{
  return characters;
}


void Physics::debug()
{
  debug_toggle = !debug_toggle;
}


void Physics::init()
{
  debug_drawer.init();
}


void Physics::pause()
{
  pause_toggle = !pause_toggle;
}


Raycast_Hitpoint Physics::ray_pick(glm::vec3 &out_origin, glm::vec3 &direction)
{
  glm::vec3 end;

  vec3 zero_vector = vec3(0, 0, 0);
  Raycast_Hitpoint hitpoint(zero_vector, direction, out_origin, 0, nullptr);

  //vec3 out_direction = direction * 1000.0f;
  vec3 out_direction = direction;

  //btCollisionWorld::ClosestRayResultCallback RayCallback(btVector3(out_origin.x, out_origin.y, out_origin.z), 
  //    btVector3(out_direction.x, out_direction.y, out_direction.z));

  // btCollisionWorld::AllHitsRayResultCallback RayCallback(btVector3(out_origin.x, out_origin.y, out_origin.z), 
  auto ray_callback = std::unique_ptr<Physics_Custom_AllHitsRayResultCallback>(new Physics_Custom_AllHitsRayResultCallback(
        btVector3(out_origin.x, out_origin.y, out_origin.z),
        btVector3(out_direction.x, out_direction.y, out_direction.z)
        ));

  world->rayTest(btVector3(out_origin.x, out_origin.y, out_origin.z), 
      btVector3(out_direction.x, out_direction.y, out_direction.z), *ray_callback);

  if (!ray_callback->hasHit())
    return hitpoint;

  std::vector<Raycast_Hitpoint> hitpoint_list;

  POLL_DEBUG(std::cout, "Num hitpoins: " << ray_callback->m_collisionObjects.size());
  for (int i = 0; i < ray_callback->m_hitPointWorld.size(); i++) {
    Node *node_ptr = (Node *) ray_callback->m_collisionObjects[i]->getUserPointer();
    vec3 v = vec3(ray_callback->m_hitPointWorld[i].getX(), ray_callback->m_hitPointWorld[i].getY(), ray_callback->m_hitPointWorld[i].getZ());
    float len = glm::length(out_origin - v);

    hitpoint.node_ptr = node_ptr;
    hitpoint.world_hitpoint = v;
    hitpoint.length = len;
    hitpoint_list.push_back(hitpoint);
    POLL_DEBUG(std::cout, "The ray hit: " << node_ptr->name_get() << " world pos: " << glm::to_string(v) << " len: " << len);
  }

  std::sort(hitpoint_list.begin(), hitpoint_list.end());

  POLL_DEBUG(std::cout, "After sorting");
  for (auto &h : hitpoint_list) {
    POLL_DEBUG(std::cout, "The ray hit: " << h.node_ptr->name_get() << " world pos: " << 
        glm::to_string(h.world_hitpoint) << " len: " << h.length << " priority: " << h.node_ptr->raycast_priority_get());
  }

  auto first = *hitpoint_list.begin();
  POLL_DEBUG(std::cout, "Closest hit: " << first.node_ptr->name_get() << " world pos: " << 
      glm::to_string(first.world_hitpoint) << " len: " << first.length);

  return first;
}


void Physics::rigidbody_add(Physics_Rigidbody *rigidbody)
{
  btRigidBody *rb_ptr = rigidbody->bt_rigidbody_get();
  world->addRigidBody(rb_ptr);
}


void Physics::rigidbody_delete(Physics_Rigidbody *rigidbody)
{
  btRigidBody *rb_ptr = rigidbody->bt_rigidbody_get();
  world->removeCollisionObject(rb_ptr);
  world->removeRigidBody(rb_ptr);
}


void Physics::rigidbody_constraint_add(Physics_Rigidbody *rigidbody)
{
  auto dof6 = rigidbody->bt_dof6_get();
  world->addConstraint(dof6, true);
}


void Physics::rigidbody_constraint_delete(Physics_Rigidbody *rigidbody)
{
  auto dof6 = rigidbody->bt_dof6_get();
  world->removeConstraint(dof6);
}


void Physics::step(Scene &scene, const double dt)
{
  if (custom_step_callback)
    custom_step_callback();

  bullet_step(scene, dt);
}


void Physics::term()
{
  debug_drawer.term();
}


/**************************************************/
/***************** PRIVATE METHODS ****************/
/**************************************************/



void Physics::bullet_init()
{
  collision_config= new btDefaultCollisionConfiguration();
  dispatcher = new btCollisionDispatcher(collision_config);
  solver = new btSequentialImpulseConstraintSolver;

  btVector3 worldMin(-1000,-1000,-1000);
  btVector3 worldMax(1000,1000,1000);
  sweep_bp = new btAxisSweep3(worldMin, worldMax);
  //  broadphase = new btDbvtBroadphase();
  sweep_bp->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
  // broadphase->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());

  //world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collision_config);
  world = new btDiscreteDynamicsWorld(dispatcher, sweep_bp, solver, collision_config);
  world->getDispatchInfo().m_allowedCcdPenetration=0.0001f;

  //broadphase->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
  sweep_bp->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());

  // broadphase filter callback
  // btOverlapFilterCallback *filterCallback = new FilterCallback();
  // world->getPairCache()->setOverlapFilterCallback(filterCallback);

  //
  world->setGravity(btVector3(0, -10, 0));
  world->setDebugDrawer(&debug_drawer);
  debug_drawer.setDebugMode(btIDebugDraw::DBG_DrawWireframe | btIDebugDraw::DBG_DrawAabb); 
}


Physics_Character_Controller *Physics::bullet_kinematic_character_controller_create(Node &node, Node &collision_node)
{
  Physics_Character_Controller *character_ptr = nullptr;
  /*
     btCollisionShape *fallShape = bullet_collision_shape_convex_hull_create(collision_node);

     btTransform startTransform;
     startTransform.setIdentity();
     startTransform.setFromOpenGLMatrix((btScalar *) &node.transform_model_get());

     btPairCachingGhostObject *actorGhost = new btPairCachingGhostObject();
     actorGhost->setUserPointer((void*) &node);
     actorGhost->setWorldTransform(startTransform);
     actorGhost->setCollisionShape(fallShape);
     actorGhost->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);
     world->addCollisionObject(actorGhost, E_Actor, E_Static | E_Riggid | E_Actor | E_Trigger);

     std::unique_ptr<Physics_Character_Controller> character(new Physics_Character_Controller(actorGhost,static_cast<btConvexShape *>(fallShape), 2.0f));
     character_ptr = character.get();
     character_ptr->node_set(node);
     character_ptr->reset();
     characters.push_back(std::move(character));
     world->addAction(character_ptr);
     */

  return character_ptr;
}


int Physics::bullet_step(Scene &scene, const double dt)
{
  Stock_Shaders shader = scene.assets_get().stock_shaders_get();
  float timestep = 1.f / 60.f;
  //float timestep = (float) dt;
  int max_sub_steps = 1;
  float fixed_time_step = 1.f / 60.f;


  if (pause_toggle) {
    for (auto &character : characters) {
      character->bullet_character_step(dt);
    }
    //std::cout << "timeStep <  maxSubSteps * fixedTimeStep: " << timestep << " < " << max_sub_steps * fixed_time_step << std::endl;
    world->stepSimulation(timestep, max_sub_steps, fixed_time_step);
  }

  if (debug_toggle) {
    shader.world_physics_debug.use();
    world->debugDrawWorld();

    debug_drawer.drawLine(btVector3(0, 0, 0), btVector3(1, 0, 0), btVector3(1, 0, 0), btVector3(1, 0, 0));
    debug_drawer.drawLine(btVector3(0, 0, 0), btVector3(0, 1, 0), btVector3(0, 1, 0), btVector3(0, 1, 0));
    debug_drawer.drawLine(btVector3(0, 0, 0), btVector3(0, 0, 1), btVector3(0, 0, 1), btVector3(0, 0, 1));

    debug_drawer.draw();
  }

  /*
     for (auto &character : characters) {
     character->bullet_debug_draw_contacts(world, sweep_bp);
     }
     */

  return debug_toggle;
}


void Physics::bullet_term()
{
  if (world) delete world;
  if (solver) delete solver;
  if (dispatcher) delete dispatcher;
  if (collision_config) delete collision_config;
  if (broadphase) delete broadphase;
  if (sweep_bp) delete sweep_bp;
}


/**************************************************/
/************** CUSTOM MOTION STATE ***************/
/**************************************************/


Physics_Motion_State::Physics_Motion_State(Node &node)
{
  transform.setIdentity();
  node_set(node);
}


void Physics_Motion_State::node_set(Node &node)
{
  Physics_Rigidbody *rigidbody = node.physics_rigidbody_get();
  auto shape = rigidbody->shape_get();
  auto &bt_shape = shape->bt_collision_shape_get();

  glm::vec3 scale = node.scale_global_get();
  bt_shape.setLocalScaling(btVector3(scale.x, scale.y, scale.z));

  glm::mat4 m = node.transform_global_translate_get() * node.transform_global_rotate_get();
  this->transform.setIdentity();
  this->transform.setFromOpenGLMatrix((btScalar *) &m);

  this->node = &node;
}


void Physics_Motion_State::setWorldTransform(const btTransform &t)
{
  glm::mat4 m;
  if (!node) return;

  glm::mat4 transform_global_scale = node->transform_global_scale_get();
  t.getOpenGLMatrix((btScalar *) &m);
  node->transform_global_set(m * transform_global_scale);
}


void Physics_Motion_State::getWorldTransform(btTransform &t) const
{
  t = transform;
}

