#include "physics.h"


/**************************************************/
/***************** CONSTRUCTORS *******************/
/**************************************************/


Physics::Physics()
{
  pause_toggle = false;
  bullet_init();
}


Physics::~Physics()
{
  bullet_term();
}


/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/


void Physics::collision_node_add(Node &node, const Physics_Collision_Shape shape, bool recursive)
{
  Physics_Node p_node;

  if (!node.mesh) {
    std::cout << "No mesh for node: '" << node.name << "', skipping ..." << std::endl;
  } else {

    p_node.node = &node;
    p_node.rigidbody = bullet_collision_rigidbody_create(node, shape);
    p_nodes.push_back(p_node);
    bullet_world_add(p_node);

    std::cout << "Added collision shape " << shape << " for node: " << node.name << std::endl;
  }

  if (!recursive)
    return;

  for (auto &child : node.children) {
    collision_node_add(*child, shape, recursive);
  }

}


void Physics::collision_node_callback_set(const Node &node, const std::function<void (int)> callback)
{
  std::cout << "Added cb for node: " << node.name << std::endl;
  callback(1);
}


void Physics::pause()
{
  pause_toggle = !pause_toggle;
}

void Physics::step(const Uint32 dt)
{
  bullet_step(dt);
}


/**************************************************/
/***************** PRIVATE METHODS ****************/
/**************************************************/


btRigidBody *Physics::bullet_collision_rigidbody_create(Node &node, Physics_Collision_Shape shape)
{
  btCollisionShape *collision_shape = nullptr;

  switch (shape) {
    case PHYSICS_COLLISION_SPHERE:
      collision_shape = new btSphereShape(1);
      break;
    case PHYSICS_COLLISION_BOX:
      break;
    default:
      break;
  }

  Physics_Motion_State *motion_state = new Physics_Motion_State(btTransform(btQuaternion(0, 0, 0, 1)), node);

  btScalar mass = 1;
  btVector3 inertia(0, 0, 0);
  collision_shape->calculateLocalInertia(mass, inertia);
  btRigidBody::btRigidBodyConstructionInfo rb_ci(mass, motion_state, collision_shape, inertia);

  btRigidBody* rb = new btRigidBody(rb_ci);

  return rb;
}


void Physics::bullet_collision_rigidbody_delete(btRigidBody *rb)
{
  delete rb;
}


void Physics::bullet_init()
{
  broadphase = new btDbvtBroadphase();
  collision_config= new btDefaultCollisionConfiguration();
  dispatcher = new btCollisionDispatcher(collision_config);
  solver = new btSequentialImpulseConstraintSolver;
  world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collision_config);

  world->setGravity(btVector3(0,-1,0));


  // Options avaliable:
  // btIDebugDraw::DBG_DrawWireframe 
  // btIDebugDraw::DBG_DrawAabb 
  // btIDebugDraw::DBG_DrawConstraints 
  // btIDebugDraw::DBG_DrawConstraintLimits 
  // Page 16 in manual
  world->setDebugDrawer(&debug_drawer);
  debug_drawer.setDebugMode(btIDebugDraw::DBG_DrawWireframe | btIDebugDraw::DBG_DrawAabb); 
}


void Physics::bullet_step(const Uint32 dt)
{
  if (!pause_toggle) {
    world->stepSimulation((double) dt / 1000.0, 10);
  }

  world->debugDrawWorld();
}


void Physics::bullet_term()
{
  delete world;
  delete solver;
  delete dispatcher;
  delete collision_config;
  delete broadphase;
}


void Physics::bullet_world_add(Physics_Node &p_node)
{
  world->addRigidBody(p_node.rigidbody);
}


void Physics::bullet_world_delete(Physics_Node &p_node)
{
  world->removeRigidBody(p_node.rigidbody);
}


/**************************************************/
/************** CUSTOM MOTION STATE ***************/
/**************************************************/


Physics_Motion_State::Physics_Motion_State(const btTransform &start_position, Node &node)
{
  this->node = &node;
}


void Physics_Motion_State::getWorldTransform(btTransform &t) const
{
}


void Physics_Motion_State::setWorldTransform(const btTransform &t)
{
  glm::mat4 m;
  t.getOpenGLMatrix((btScalar *) &m);
  node->mesh->model = m;
}

