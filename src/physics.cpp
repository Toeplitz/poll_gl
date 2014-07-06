#include "physics.h"
#include "utils.h"
#include <glm/gtx/string_cast.hpp>

const int NUM_VERTS_X = 30;
const int NUM_VERTS_Y = 30;
const int totalVerts = NUM_VERTS_X*NUM_VERTS_Y;
const float TRIANGLE_SIZE=8.f;

void  Physics::setVertexPositions(float waveheight, float offset)
{
  int i;
  int j;

  for ( i=0;i<NUM_VERTS_X;i++)
  {
    for (j=0;j<NUM_VERTS_Y;j++)
    {
      gVertices[i+j*NUM_VERTS_X].setValue((i-NUM_VERTS_X*0.5f)*TRIANGLE_SIZE,
          //0.f,
          waveheight*sinf((float)i+offset)*cosf((float)j+offset),
          (j-NUM_VERTS_Y*0.5f)*TRIANGLE_SIZE);
    }
  }
}


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


void Physics::collision_mesh_add(Node &node, const std::string &prefix, const std::string &filename)
{
  Model model;
  Node *root_ptr = model.load(collision_assets, node, prefix, filename,  false);
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

  //rb->setCollisionFlags(rb->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
  // rb->setCollisionFlags(rb->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);


  return rb;
}


void Physics::bullet_collision_rigidbody_delete(btRigidBody *rb)
{
  delete rb;
}


btCollisionShape *Physics::bullet_collision_shape_convex_hull_create(Node &node)
{
  btCollisionShape *collision_shape = nullptr;
  std::vector<glm::vec3> vertices = node.mesh->vertices_get(false);
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
  std::vector<glm::vec3> vertices = node.mesh->vertices_get(false);
  std::vector<int> indices = node.mesh->indices_get();

  if  (mesh) {
    std::vector<glm::vec4> vertices;
    std::vector<glm::vec4> normals;
    std::vector<glm::vec4> tangents;
    std::vector<glm::vec4> bitangents;
    std::vector<glm::vec4> weights;
    std::vector<glm::ivec4> bone_indices;
    std::vector<glm::vec2> uvs;
    std::vector<GLshort> indices;
    mesh->buffer_data_get(&vertices, &normals, &tangents, &bitangents,
        &weights, &bone_indices, &uvs, &indices);

    int n = node.mesh->num_vertices_get();
    int j = node.mesh->num_indices_get();
    std::cout << "vertices: " << n << std::endl;
    std::cout << "indices: " << j << std::endl;
    std::cout << "indices / 3: " << j / 3 << std::endl;
    std::cout << "num faces: " << node.mesh->num_faces << std::endl;
    bt_vert = new btVector3[n];
    ind = new int[j];
    for (size_t i = 0; i < (size_t) n; i++) {
      bt_vert->setX(vertices[i].x);
      bt_vert->setY(vertices[i].y);
      bt_vert->setZ(vertices[i].z);
    }
    for (size_t i = 0; i < (size_t) j; i++) {
      ind[i] = indices[i];
    }

    int vertStride = sizeof(btVector3);
    int indexStride = 3 * sizeof(int);

    assert(node.mesh->num_indices_get() % 3 == 0);
    assert(node.mesh->num_faces == j / 3);

    btTriangleMesh* ptrimesh = new btTriangleMesh();
    for (unsigned int i = 0; i < node.mesh->num_faces; i = i + 3) {
      std::cout << "Triangle points (x, y, z) , (x, y, z) , (x, y, z): " << std::endl;
      std::cout << "(" << bt_vert[ind[i]].x() << ", " << bt_vert[ind[i]].y() << ", " << bt_vert[ind[i]].z() << ") ";
      std::cout << "(" << bt_vert[ind[i + 1]].x() << ", " << bt_vert[ind[i + 1]].y() << ", " << bt_vert[ind[i + 1]].z() << ") ";
      std::cout << "(" << bt_vert[ind[i + 2]].x() << ", " << bt_vert[ind[i + 2]].y() << ", " << bt_vert[ind[i + 2]].z() << ") " << std::endl;
      ptrimesh->addTriangle(bt_vert[ind[i]], bt_vert[ind[i + 1]], bt_vert[ind[i + 2]]);

    }

    /*
       float rx = 2.0f;
       float ry = 2.0f;
       float rz = 2.0f;

       const int nverts = 8;
       const btVector3 verts[nverts] = {
       btVector3(-rx, -ry, -rz),
       btVector3(-rx, -ry, rz),
       btVector3(-rx, ry, -rz),
       btVector3(-rx, ry, rz),
       btVector3(rx, -ry, -rz),
       btVector3(rx, -ry, rz),
       btVector3(rx, ry, -rz),
       btVector3(rx, ry, rz)};

       const int nfaces = 12;
       int faces[nfaces][3] = {
       {0,1,2},
       {3,1,2},
       {0,1,4},
       {5,1,4},
       {0,2,4},
       {6,2,4},
       {7,6,5},
       {4,6,5},
       {7,6,3},
       {2,6,3},
       {7,5,3},
       {1,5,3}};

       btTriangleMesh* ptrimesh = new btTriangleMesh();
       for (int i=0; i < nfaces; ++i) {
       std::cout << "triangle: x, y, z = " << verts[faces[i][0]] << ", " << verts[faces[i][1]] << ", " << verts[faces[i][1]] << std::endl;
       ptrimesh->addTriangle(verts[faces[i][0]], verts[faces[i][1]], verts[faces[i][2]]);
       }
       */

    /*
       array = new btTriangleIndexVertexArray(node.mesh->num_faces, 
       ind, indexStride, 
       n, (btScalar *) &bt_vert[0].x(), vertStride);
       */

    /*
       const int totalTriangles = 2*(NUM_VERTS_X-1)*(NUM_VERTS_Y-1);

       gVertices = new btVector3[totalVerts];
       gIndices = new int[totalTriangles*3];

       setVertexPositions(5.0f,0.f);

       int index=0;
       for (int i=0;i<NUM_VERTS_X-1;i++)
       {
       for (int j=0;j<NUM_VERTS_Y-1;j++)
       {
       gIndices[index++] = j*NUM_VERTS_X+i;
       gIndices[index++] = j*NUM_VERTS_X+i+1;
       gIndices[index++] = (j+1)*NUM_VERTS_X+i+1;

       gIndices[index++] = j*NUM_VERTS_X+i;
       gIndices[index++] = (j+1)*NUM_VERTS_X+i+1;
       gIndices[index++] = (j+1)*NUM_VERTS_X+i;
       }
       }

       std::cout << "Example totalVerts: " << totalVerts << std::endl;
       std::cout << "Example triangles: " << totalTriangles << std::endl;
       std::cout << "Example total indices: " << totalTriangles * 3 << std::endl;
       array = new btTriangleIndexVertexArray(totalTriangles,
       gIndices,
       indexStride,
       totalVerts, (btScalar*) &gVertices[0].x(), vertStride);

*/

    bool useQuantizedAabbCompression = true;
    btVector3 aabbMin(-1000, -1000, -1000), aabbMax(1000, 1000, 1000);
    //  collision_shape = new btBvhTriangleMeshShape(array, useQuantizedAabbCompression, aabbMin, aabbMax);
    collision_shape = new btBvhTriangleMeshShape(ptrimesh, true);
    collision_shape->setLocalScaling(btVector3(node.original_scaling.x, node.original_scaling.y, node.original_scaling.z));
  } else {
    std::cout << "ERROR: no mesh for node: " << node.name << std::endl;
  }

  return collision_shape;
}


void Physics::bullet_init()
{
  collision_config= new btDefaultCollisionConfiguration();
  dispatcher = new btCollisionDispatcher(collision_config);
  solver = new btSequentialImpulseConstraintSolver;

  btVector3 worldMin(-1000,-1000,-1000);
  btVector3 worldMax(1000,1000,1000);
  btAxisSweep3 *sweep_bp = new btAxisSweep3(worldMin,worldMax);
  overlapping_pair_cache = sweep_bp;
  //broadphase = new btDbvtBroadphase();

  //world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collision_config);
  world = new btDiscreteDynamicsWorld(dispatcher, overlapping_pair_cache, solver, collision_config);
  //  world->getDispatchInfo().m_allowedCcdPenetration=0.0001f;


  world->setGravity(btVector3(0, -9.81, 0));
  world->setDebugDrawer(&debug_drawer);
  debug_drawer.setDebugMode(btIDebugDraw::DBG_DrawWireframe | btIDebugDraw::DBG_DrawAabb); 
}


void Physics::bullet_kinematic_character_controller_create(Node &node)
{
  btKinematicCharacterController *m_character; 
  btPairCachingGhostObject *m_ghostObject;

  if (node.mesh) {
    btTransform startTransform;
    //startTransform.setIdentity ();
    //startTransform.setOrigin(btVector3(node.original_position.x, node.original_position.y, node.original_position.z));
    std::cout << "Adding character controller for node: '" << node.name << "'" << std::endl;
    startTransform.setFromOpenGLMatrix((btScalar *) &node.mesh->model);
    print_matrix(std::cout, node.mesh->model, 0);

    m_ghostObject = new btPairCachingGhostObject();
    m_ghostObject->setWorldTransform(startTransform);
    overlapping_pair_cache->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
    btScalar characterHeight = 1;
    btScalar characterWidth = 1;
    btConvexShape* capsule = new btCapsuleShape(characterWidth,characterHeight);
    m_ghostObject->setCollisionShape (capsule);
    m_ghostObject->setCollisionFlags (btCollisionObject::CF_CHARACTER_OBJECT);

    btScalar stepHeight = btScalar(0.35);
    m_character = new btKinematicCharacterController(m_ghostObject, capsule, stepHeight);

    // only collide with static for now (no interaction with dynamic objects)
    world->addCollisionObject(m_ghostObject ,btBroadphaseProxy::CharacterFilter, 
        btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter);

    world->addAction(m_character);
  }

  for (auto &child : node.children) {
    bullet_kinematic_character_controller_create(*child);
  }
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

