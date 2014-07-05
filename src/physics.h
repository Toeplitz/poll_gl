#pragma once

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/BroadphaseCollision/btAxisSweep3.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletCollision/CollisionShapes/btTriangleIndexVertexArray.h>
#include <SDL2/SDL_stdinc.h>            // for Uint32
#include <functional>
#include "assets.h"
#include "camera.h"
#include "node.h"
#include "glcontext.h"
#include "gldebug.h"
#include "utils.h"

//
// User manual:
// http://www.cs.uu.nl/docs/vakken/mgp/assignment/Bullet%20-%20User%20Manual.pdf
//
//


enum Physics_Collision_Shape 
{
  PHYSICS_COLLISION_BOX,
  PHYSICS_COLLISION_SPHERE,
  PHYSICS_COLLISION_CONVEX_HULL,
  PHYSICS_COLLISION_TRIANGLE_MESH
};


typedef struct 
{
  Node *node;
  btRigidBody *rigidbody;
} Physics_Node;


class Physics_Motion_State: public btMotionState 
{
  private:
    Node *node;
    btTransform transform;

  public:
    Physics_Motion_State(const btTransform &start_position, Node &node);

    virtual void getWorldTransform(btTransform &t) const;
    virtual void setWorldTransform(const btTransform &t);
};


class Physics 
{
  class btBroadphaseInterface *overlapping_pair_cache;

  private:
    btBroadphaseInterface                *broadphase;
    btDefaultCollisionConfiguration      *collision_config;
    btCollisionDispatcher                *dispatcher;
    btSequentialImpulseConstraintSolver  *solver;
    btDiscreteDynamicsWorld              *world;

    GLshader                              glshader;
    std::vector<Physics_Node>             p_nodes;
    Physics_Debug_Drawer                  debug_drawer;
    int                                   debug_toggle;
    int                                   pause_toggle;
    Assets                                collision_assets;

    btRigidBody      *bullet_collision_rigidbody_create(Node &node, Physics_Collision_Shape shape, float m);
    void              bullet_collision_rigidbody_delete(btRigidBody *rb);
    btCollisionShape *bullet_collision_shape_convex_hull_create(Node &node);
    btCollisionShape *bullet_collision_shape_triangle_mesh_create(Node &node);
    void              bullet_init();
    int               bullet_step(const Uint32 dt);
    void              bullet_term();
    void              bullet_world_add(Physics_Node &p_node);
    void              bullet_world_delete(Physics_Node &p_node);

  public:
    Physics();
    ~Physics();

    void bullet_kinematic_character_controller_create(Node &node);
    
    void collision_mesh_add(Node &node, const std::string &prefix, const std::string &filename);
    void collision_node_add(Node &node, const Physics_Collision_Shape shape, bool recursive, float mass);
    void collision_node_callback_set(const Node &node, const std::function<void (int)> callback);
    void debug();
    void init();
    void pause();
    void step(const Uint32 dt);
};

