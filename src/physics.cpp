#include "node.h"
#include "physics.h"
#include "physics_char_cont.h"
#include "physics_rigidbody.h"
#include "raycast.h"
#include "scene.h"
#include "utils.h"
#include <glm/gtx/string_cast.hpp>



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


std::shared_ptr<Raycast_Hitpoint> Physics::ray_pick(const glm::vec3 &out_origin, const glm::vec3 &direction)
{
  glm::vec3 end;

  auto hitpoint = std::shared_ptr<Raycast_Hitpoint>(new Raycast_Hitpoint);
  hitpoint->node_ptr = nullptr;
  hitpoint->world_hitpoint = vec3(0, 0, 0);

  vec3 out_direction = direction * 1000.0f;

  btCollisionWorld::ClosestRayResultCallback RayCallback(btVector3(out_origin.x, out_origin.y, out_origin.z), 
      btVector3(out_direction.x, out_direction.y, out_direction.z));
  world->rayTest(btVector3(out_origin.x, out_origin.y, out_origin.z), 
      btVector3(out_direction.x, out_direction.y, out_direction.z), RayCallback);

  if (!RayCallback.hasHit())
    return nullptr;

  end = glm::vec3(RayCallback.m_hitPointWorld.getX(), RayCallback.m_hitPointWorld.getY(), RayCallback.m_hitPointWorld.getZ());
  Node *node_ptr = (Node *) RayCallback.m_collisionObject->getUserPointer();
  hitpoint->node_ptr = node_ptr;
  hitpoint->world_hitpoint = end;
  return hitpoint;
}


void Physics::rigidbody_add(Physics_Rigidbody *rigidbody)
{
  btRigidBody *rb_ptr = rigidbody->bt_rigidbody_get();
  POLL_DEBUG(std::cout, "adding rigidbody for: " << rigidbody->node_ptr_get()->name_get());
  world->addRigidBody(rb_ptr);
}


void Physics::rigidbody_delete(Physics_Rigidbody *rigidbody)
{
  btRigidBody *rb_ptr = rigidbody->bt_rigidbody_get();
  POLL_DEBUG(std::cout, "removing rigidbody for: " << rigidbody->node_ptr_get()->name_get());
  world->removeRigidBody(rb_ptr);
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
  Physics_Character_Controller *character_ptr;
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

  shader.world_physics_debug.use();
  world->debugDrawWorld();

  debug_drawer.drawLine(btVector3(0, 0, 0), btVector3(1, 0, 0), btVector3(1, 0, 0), btVector3(1, 0, 0));
  debug_drawer.drawLine(btVector3(0, 0, 0), btVector3(0, 1, 0), btVector3(0, 1, 0), btVector3(0, 1, 0));
  debug_drawer.drawLine(btVector3(0, 0, 0), btVector3(0, 0, 1), btVector3(0, 0, 1), btVector3(0, 0, 1));

  debug_drawer.draw();
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


Physics_Motion_State::Physics_Motion_State(const btTransform &start_position, Node &node)
{
  node_set(node);
}


void Physics_Motion_State::node_set(Node &node)
{
  glm::mat4 model = node.transform_global_get();
  this->transform.setFromOpenGLMatrix((btScalar *) &model);
  this->node = &node;
}


void Physics_Motion_State::setWorldTransform(const btTransform &t)
{
  glm::mat4 m;

  if (!node) return;

  t.getOpenGLMatrix((btScalar *) &m);
  node->transform_global_set(m);
}


void Physics_Motion_State::transform_set(const glm::mat4 &model)
{
  transform.setFromOpenGLMatrix((btScalar *) &model);
}


void Physics_Motion_State::getWorldTransform(btTransform &t) const
{
  t = transform;
}

