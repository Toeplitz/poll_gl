#include "physics.h"

static glm::mat4    bullet_convert_glm(btTransform transform);
static btTransform  bullet_convert_transform(glm::mat4 transform);

/**************************************************/
/***************** CONSTRUCTORS *******************/
/**************************************************/


Physics::Physics():
  pause_toggle(false)
{
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
/***************** STATIC FUNCTIONS ***************/
/**************************************************/

static glm::mat4 bullet_convert_glm(btTransform transform)
{
  float data[16];
  transform.getOpenGLMatrix(data);
  return glm::make_mat4(data);
}


static btTransform bullet_convert_transform(glm::mat4 transform)
{
  const float *data = glm::value_ptr(transform);
  btTransform bulletTransform;
  bulletTransform.setFromOpenGLMatrix(data);
  return bulletTransform;
}

/**************************************************/
/***************** PRIVATE METHODS ****************/
/**************************************************/



btRigidBody *Physics::bullet_collision_rigidbody_create(Node &node, Physics_Collision_Shape shape)
{
  btCollisionShape *collision_shape = nullptr;

  switch (shape) {
    case PHYSICS_COLLISION_SPHERE:
      collision_shape = new btSphereShape(btScalar(1));
      break;
    case PHYSICS_COLLISION_BOX:
      collision_shape = new btBoxShape(btVector3(1, 1, 1));
      break;
    default:
      break;
  }

  if (!collision_shape) {
    std::cout << "Error: No valid shape found" << std::endl;
    return nullptr;
  }

  btTransform t = bullet_convert_transform(node.mesh->model);
  printMatrix(std::cout, node.mesh->model, 0);
  Physics_Motion_State *motion_state = new Physics_Motion_State(btTransform(btQuaternion(0,0,0,1),btVector3(0,0,0)), node);
  //Physics_Motion_State *motion_state = new Physics_Motion_State(t, node);

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

  world->setGravity(btVector3(0, -9.81, 0));
  world->setDebugDrawer(&debug_drawer);
  debug_drawer.setDebugMode(btIDebugDraw::DBG_DrawWireframe | btIDebugDraw::DBG_DrawAabb); 
}


void Physics::bullet_step(const Uint32 dt)
{
  if (pause_toggle) {
    world->stepSimulation(1./1000., 10);
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
  this->transform = start_position;
  this->node = &node;
}


void Physics_Motion_State::getWorldTransform(btTransform &t) const
{
  t = transform;
}


void Physics_Motion_State::setWorldTransform(const btTransform &t)
{
  if (!node) return;

  glm::mat4 m = bullet_convert_glm(t);
  printMatrix(std::cout, m, 0);
  node->mesh->model = m;
}

