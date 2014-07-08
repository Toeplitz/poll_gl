#include "physics.h"
#include "physics_char_cont.h"
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
  bullet_init();
}


Physics::~Physics()
{
  bullet_term();
}


/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/

Physics_CharacterController *Physics::character_controller_add(Node &node, Node &collision_node)
{
  if (!node.mesh) {
    std::cout << "Error: no mesh for character controller creation (node: '" << node.name << "')" << std::endl;
    return nullptr;
  }

  return bullet_kinematic_character_controller_create(node, collision_node);
}


Physics_CharacterController_List const  &Physics::character_get_all() const
{
  return characters;
}


void Physics::character_controller_remove(Physics_CharacterController *char_cont)
{
}


void character_controller_move(Physics_CharacterController *char_cont, Physics_Direction dir)
{
}


void character_controller_jump(Physics_CharacterController *char_cont)
{
}


void Physics::collision_mesh_add(Node &node, const std::string &prefix, const std::string &filename)
{
  Model model;
  Node *root_ptr = model.load(collision_assets, node, prefix, filename);
  collision_assets.print_all();
  std::cout << "Root ptr collision: " << root_ptr->name << std::endl;
  for (auto &armature: collision_assets.armature_get_all()) {
    for (auto &bone: armature->bones_get_all()) {
      std::cout << bone->joint_node->name << std::endl;
      // Here we have the name of the bone in the real mesh.
    }
  }

}


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
    case PHYSICS_COLLISION_TRIANGLE_MESH:
      collision_shape = bullet_collision_shape_triangle_mesh_create(node);
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
  std::vector<glm::vec3> vertices = node.mesh->positions_get();
  int n = vertices.size();

  std::cout << "Convex hull Num: " << vertices.size() << std::endl;
  collision_shape = new btConvexHullShape((btScalar *) vertices.data(), n, sizeof(glm::vec3));
  collision_shape->setLocalScaling(btVector3(node.original_scaling.x, node.original_scaling.y, node.original_scaling.z));

  return collision_shape;
}


btCollisionShape *Physics::bullet_collision_shape_triangle_mesh_create(Node &node)
{
  Mesh *mesh = node.mesh;
  btCollisionShape *collision_shape = nullptr;

  if  (!mesh) {
    std::cout << "ERROR: no mesh for node: " << node.name << std::endl;
    return nullptr;
  }

  std::vector<glm::vec3> positions = mesh->positions_get();
  std::vector<GLshort> indices = mesh->indices_get();

  assert(node.mesh->num_indices_get() % 3 == 0);

  btTriangleMesh* ptrimesh = new btTriangleMesh();
  for (unsigned int i = 0; i < node.mesh->num_indices_get(); i = i + 3) {
    /*
    std::cout << "Triangle points (x, y, z) , (x, y, z) , (x, y, z): " << std::endl;
    std::cout << "(" << positions[indices[i]].x << ", " << positions[indices[i]].y << ", " << positions[indices[i]].z << ") ";
    std::cout << "(" << positions[indices[i + 1]].x << ", " << positions[indices[i + 1]].y << ", " << positions[indices[i + 1]].z << ") ";
    std::cout << "(" << positions[indices[i + 2]].x << ", " << positions[indices[i + 2]].y << ", " << positions[indices[i + 2]].z << ") " << std::endl;
    */

    ptrimesh->addTriangle(btVector3(positions[indices[i]].x, positions[indices[i]].y, positions[indices[i]].z),
        btVector3(positions[indices[i + 1]].x, positions[indices[i + 1]].y, positions[indices[i + 1]].z),
        btVector3(positions[indices[i + 2]].x, positions[indices[i + 2]].y, positions[indices[i + 2]].z));
  }

  bool useQuantizedAabbCompression = true;
  btVector3 aabbMin(-1000, -1000, -1000), aabbMax(1000, 1000, 1000);
  collision_shape = new btBvhTriangleMeshShape(ptrimesh, useQuantizedAabbCompression, aabbMin, aabbMax);
  collision_shape->setLocalScaling(btVector3(node.original_scaling.x, node.original_scaling.y, node.original_scaling.z));

  return collision_shape;
}


void Physics::bullet_init()
{
  collision_config= new btDefaultCollisionConfiguration();
  dispatcher = new btCollisionDispatcher(collision_config);
  solver = new btSequentialImpulseConstraintSolver;

  btVector3 worldMin(-1000,-1000,-1000);
  btVector3 worldMax(1000,1000,1000);
  sweep_bp = new btAxisSweep3(worldMin, worldMax);
  //broadphase = new btDbvtBroadphase();
  sweep_bp->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
//  broadphase->getOverlappingPairCache()->setInternalGhostPairCallback(actorGhostPairCallback);

  //world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collision_config);
  world = new btDiscreteDynamicsWorld(dispatcher, sweep_bp, solver, collision_config);
  world->getDispatchInfo().m_allowedCcdPenetration=0.0001f;

  //broadphase->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
  //sweep_bp->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());

  // broadphase filter callback
  btOverlapFilterCallback * filterCallback = new FilterCallback();
  world->getPairCache()->setOverlapFilterCallback(filterCallback);

  world->setGravity(btVector3(0, -10, 0));
  world->setDebugDrawer(&debug_drawer);
  debug_drawer.setDebugMode(btIDebugDraw::DBG_DrawWireframe | btIDebugDraw::DBG_DrawAabb); 
}


Physics_CharacterController *Physics::bullet_kinematic_character_controller_create(Node &node, Node &collision_node)
{
  Physics_CharacterController *character_ptr;
  btCollisionShape *fallShape = bullet_collision_shape_convex_hull_create(collision_node);

  btTransform startTransform;
  startTransform.setFromOpenGLMatrix((btScalar *) &node.mesh->model);

  btPairCachingGhostObject *actorGhost = new btPairCachingGhostObject();
  actorGhost->setUserPointer((void*) &node);
  actorGhost->setWorldTransform(startTransform);
  actorGhost->setCollisionShape(fallShape);
  actorGhost->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);
  world->addCollisionObject(actorGhost, E_Actor, E_Static | E_Riggid | E_Actor | E_Trigger);

  std::unique_ptr<Physics_CharacterController> character(new btKinematicCharacterController(actorGhost,
        static_cast<btConvexShape *>(fallShape), 0.5f));
  character_ptr = character.get();
  //character = new Physics_CharacterController(actorGhost, static_cast<btConvexShape*>(fallShape), 0.5f);
//  character = new btKinematicCharacterController (actorGhost, static_cast<btConvexShape*>(fallShape), 10.5f);
  characters.push_back(std::move(character));
  world->addAction(character_ptr);
 // character->set_node(node);

  return character_ptr;
}


int Physics::bullet_step(const Uint32 dt)
{
  float timestep = 1.f / 60.f;
  //float timestep = (float) dt / 1000.f;
  int max_sub_steps = 1;
  float fixed_time_step = 1.f / 60.f;


  //std::cout << "timeStep <  maxSubSteps * fixedTimeStep: " << timestep << " < " << max_sub_steps * fixed_time_step << std::endl;
  if (pause_toggle) {
    for (auto &character : characters) {
      btPairCachingGhostObject *ghost = character->getGhostObject();
      Node *node = (Node *) ghost->getUserPointer();

      glm::mat4 m;
      btTransform t = ghost->getWorldTransform();
      t.getOpenGLMatrix((btScalar *) &m);
      node->mesh->model = m;

    }
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

