#pragma once

#include <btBulletDynamicsCommon.h>
#include <SDL2/SDL_stdinc.h>            // for Uint32

class Physics 
{
  private:
    btBroadphaseInterface                *broadphase;
    btDefaultCollisionConfiguration      *collision_config;
    btCollisionDispatcher                *dispatcher;
    btSequentialImpulseConstraintSolver  *solver;
    btDiscreteDynamicsWorld              *world;

  public:
    Physics();
    ~Physics();
    
    void step(Uint32 dt);


};

