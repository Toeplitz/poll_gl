#include "physics.h"
#include "utils.h"

/**************************************************/
/***************** CONSTRUCTORS *******************/
/**************************************************/


Physics::Physics():
  debug_toggle(false),
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


void Physics::collision_node_add(Node &node, const Physics_Collision_Shape shape, bool recursive, float mass)
{
  Physics_Node p_node;

  if (!node.mesh) {
    std::cout << "No mesh for node: '" << node.name << "', skipping ..." << std::endl;
  } else {

    p_node.node = &node;
    p_node.rigidbody = bullet_collision_rigidbody_create(node, shape, mass);
    p_nodes.push_back(p_node);
    bullet_world_add(p_node);

    std::cout << "Added collision shape " << shape << " for node: " << node.name << std::endl;
  }

  if (!recursive)
    return;

  for (auto &child : node.children) {
    collision_node_add(*child, shape, recursive, mass);
  }

}


void Physics::collision_node_callback_set(const Node &node, const std::function<void (int)> callback)
{
  std::cout << "Added cb for node: " << node.name << std::endl;
  callback(1);
}

void Physics::debug()
{
  debug_toggle = !debug_toggle;
}

void Physics::init()
{
  glshader.load("shaders/debug.v", "shaders/debug.f");
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



btRigidBody *Physics::bullet_collision_rigidbody_create(Node &node, Physics_Collision_Shape shape, float m)
{
  btCollisionShape *collision_shape = nullptr;
  glm::vec3 position = node.original_position;
  glm::vec3 scaling = node.original_scaling;
  glm::quat rotation = node.original_rotation;

  switch (shape) {
    case PHYSICS_COLLISION_SPHERE:
      collision_shape = new btSphereShape(btScalar(scaling.x));
      break;
    case PHYSICS_COLLISION_BOX:
      // btTransform does not have scaling, so we need to do it here.
      collision_shape = new btBoxShape(btVector3(scaling.x, scaling.y, scaling.z));
      break;
    case PHYSICS_COLLISION_CONVEX_HULL:
      collision_shape = bullet_collision_shape_convex_hull_create(node);
      break;
    default:
      break;
  }

  if (!collision_shape) {
    std::cout << "Error: No valid shape found" << std::endl;
    return nullptr;
  }

  glm::mat4 matrix;
  btTransform t;
  t.setIdentity();
  t.setOrigin(btVector3(position.x, position.y, position.z));
  t.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w));
  btTransform t2;
  t2.setFromOpenGLMatrix((btScalar *) &node.mesh->model);
 // matrix = right_handed_to_left_handed(matrix);
  //t = bullet_convert_transform(matrix);

  Physics_Motion_State *motion_state = new Physics_Motion_State(t2, node);

  btScalar mass = m;
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


btCollisionShape *Physics::bullet_collision_shape_convex_hull_create(Node &node)
{
  btCollisionShape *collision_shape = nullptr;
  std::vector<glm::vec4> vertices = node.mesh->vertices_get(false);
  int n = vertices.size();

  std::cout << "Num: " << vertices.size() << std::endl;
  collision_shape = new btConvexHullShape((btScalar *) vertices.data(), n);
  collision_shape->setLocalScaling(btVector3(node.original_scaling.x, node.original_scaling.y, node.original_scaling.z));

  return collision_shape;
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


int Physics::bullet_step(const Uint32 dt)
{
  float timestep = 1.f / 60.f;
  //float timestep = (float) dt / 1000.f;
  int max_sub_steps = 1;
  float fixed_time_step = 1.f / 60.f;

  //std::cout << "timeStep <  maxSubSteps * fixedTimeStep: " << timestep << " < " << max_sub_steps * fixed_time_step << std::endl;
  if (pause_toggle) {
    world->stepSimulation(timestep, max_sub_steps, fixed_time_step);
  }

  if (debug_toggle) {
    glshader.use();
    debug_drawer.drawLine(btVector3(0, 0, 0), btVector3(1, 0, 0), btVector3(1, 0, 0), btVector3(1, 0, 0));
    debug_drawer.drawLine(btVector3(0, 0, 0), btVector3(0, 1, 0), btVector3(0, 1, 0), btVector3(0, 1, 0));
    debug_drawer.drawLine(btVector3(0, 0, 0), btVector3(0, 0, 1), btVector3(0, 0, 1), btVector3(0, 0, 1));
    world->debugDrawWorld();
  }

  return debug_toggle;
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
  glm::mat4 scale_matrix = glm::scale(glm::mat4(1.f), node.original_scaling);
  glm::mat4 model_no_scaling = node.mesh->model * glm::inverse(scale_matrix);
  this->transform.setFromOpenGLMatrix((btScalar *) &model_no_scaling);
  //this->transform = start_position;
  this->node = &node;
}


void Physics_Motion_State::getWorldTransform(btTransform &t) const
{
  std::cout << "GET WORLD TRANSFORM" << std::endl;
  t = transform;
}


void Physics_Motion_State::setWorldTransform(const btTransform &t)
{
  if (!node) return;

  glm::mat4 m;

//  glm::mat4 m = bullet_convert_glm(t);
  t.getOpenGLMatrix((btScalar *) &m);
  //print_matrix(std::cout, m, 0);
  node->mesh->model = m * glm::scale(glm::mat4(1.f), node->original_scaling);
  //node->mesh->model = m;
  // node->mesh->physics_matrix = right_handed_to_left_handed(m);
}

