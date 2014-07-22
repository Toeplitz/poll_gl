#pragma once

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/BroadphaseCollision/btAxisSweep3.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletCollision/CollisionShapes/btTriangleIndexVertexArray.h>
#include <BulletCollision/CollisionDispatch/btInternalEdgeUtility.h>
#include <functional>
#include "assets.h"
#include "camera.h"
#include "node.h"
#include "glcontext.h"
#include "gldebug.h"
#include "utils.h"
#include "physics_char_cont.h"

//
// User manual:
// http://www.cs.uu.nl/docs/vakken/mgp/assignment/Bullet%20-%20User%20Manual.pdf
//
//


enum EPhysicsCollisionMask {

  E_Static   = 1 << 0,
  E_Riggid   = 1 << 1,
  E_Actor      = 1 << 2,
  E_Trigger   = 1 << 3,

  E_StaticGroup   = E_Riggid | E_Actor,
  E_ActorGroup   = E_Static | E_Riggid | E_Actor | E_Trigger,
  E_RiggidGroup   = E_Static | E_Riggid | E_Actor | E_Trigger ,
  E_TriggerGroup   = E_Riggid | E_Actor
};


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



typedef std::vector<std::unique_ptr<Physics_Character_Controller>>  Physics_Character_Controller_List;

class Physics 
{
  private:
    btAxisSweep3                         *sweep_bp;

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
    Physics_Character_Controller_List     characters;


    btRigidBody                   *bullet_collision_rigidbody_create(Node &node, Physics_Collision_Shape shape, float m);
    void                           bullet_collision_rigidbody_delete(btRigidBody *rb);
    btCollisionShape              *bullet_collision_shape_convex_hull_create(Node &node);
    btCollisionShape              *bullet_collision_shape_triangle_mesh_create(Node &node);
    void                           bullet_init();
    Physics_Character_Controller  *bullet_kinematic_character_controller_create(Node &node, Node &collision_node);
    int                            bullet_step(const double dt);
    void                           bullet_term();
    void                           bullet_world_add(Physics_Node &p_node);
    void                           bullet_world_delete(Physics_Node &p_node);


    std::function <void ()> custom_step_callback;

  public:
    Physics();
    ~Physics();

    void custom_step_callback_set(const std::function<void()> callback)
    {
      custom_step_callback = callback;
    }


    Physics_Character_Controller             *character_controller_add(Node &node, Node &collision_node);
    void                                      character_controller_remove(Physics_Character_Controller *char_cont);
    Physics_Character_Controller_List const  &character_get_all() const;
    void                                      collision_mesh_add(Node &node, const std::string &prefix, const std::string &filename);
    void                                      collision_shape_add(Node &node, const Physics_Collision_Shape shape, bool recursive, float mass);
    void                                      collision_node_callback_set(const Node &node, const std::function<void (int)> callback);
    void                                      debug();
    void                                      init();
    void                                      pause();
    void                                      step(const double dt);
};

