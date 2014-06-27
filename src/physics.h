#pragma once

#include <btBulletDynamicsCommon.h>
#include <SDL2/SDL_stdinc.h>            // for Uint32
#include <functional>
#include "node.h"
#include "utils.h"


enum Physics_Collision_Shape 
{
  PHYSICS_COLLISION_BOX,
  PHYSICS_COLLISION_SPHERE
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

  public:
    Physics_Motion_State(const btTransform &start_position, Node &node);

    virtual void getWorldTransform(btTransform &t) const;
    virtual void setWorldTransform(const btTransform &t);
};


class Physics 
{
  private:
    btBroadphaseInterface                *broadphase;
    btDefaultCollisionConfiguration      *collision_config;
    btCollisionDispatcher                *dispatcher;
    btSequentialImpulseConstraintSolver  *solver;
    btDiscreteDynamicsWorld              *world;

    std::vector<Physics_Node>             p_nodes;

    btRigidBody *bullet_collision_rigidbody_create(Node &node, Physics_Collision_Shape shape);
    void         bullet_collision_rigidbody_delete(btRigidBody *rb);
    void         bullet_init();
    void         bullet_step(const Uint32 dt);
    void         bullet_term();
    void         bullet_world_add(Physics_Node &p_node);
    void         bullet_world_delete(Physics_Node &p_node);

  public:
    Physics();
    ~Physics();
    
    void collision_node_add(Node &node, const Physics_Collision_Shape shape, bool recursive);
    void collision_node_callback_set(const Node &node, const std::function<void (int)> callback);
    void step(const Uint32 dt);
};

