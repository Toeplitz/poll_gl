#pragma once

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/BroadphaseCollision/btAxisSweep3.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletCollision/CollisionShapes/btTriangleIndexVertexArray.h>
#include <BulletCollision/CollisionDispatch/btInternalEdgeUtility.h>
#include <functional>
#include "gldebug.h"
#include "physics_char_cont.h"

//
// User manual:
// http://www.cs.uu.nl/docs/vakken/mgp/assignment/Bullet%20-%20User%20Manual.pdf
//
//


class Node;
class Mesh;
class Physics_Rigidbody;
class Scene;
struct Raycast_Hitpoint;


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

    void node_set(Node &node);
    void transform_set(const glm::mat4 &model);
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

    std::vector<Physics_Node>             p_nodes;
    Physics_Debug_Drawer                  debug_drawer;
    int                                   debug_toggle;
    int                                   pause_toggle;
    Physics_Character_Controller_List     characters;


    void                           bullet_init();
    Physics_Character_Controller  *bullet_kinematic_character_controller_create(Node &node, Node &collision_node);
    int                            bullet_step(Scene &scene, const double dt);
    void                           bullet_term();


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
    void                                      debug();
    void                                      init();
    void                                      pause();
    std::shared_ptr<Raycast_Hitpoint>         ray_pick(const glm::vec3 &out_origin, const glm::vec3 &out_direction);
    void                                      rigidbody_add(Physics_Rigidbody *rigidbody);
    void                                      rigidbody_delete(Physics_Rigidbody *rigidbody);
    void                                      step(Scene &scene, const double dt);
    void                                      term();
};

