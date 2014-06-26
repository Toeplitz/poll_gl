#include "physics.h"
#include <btBulletDynamicsCommon.h>
#include <SDL2/SDL_stdinc.h>            // for Uint32

/**************************************************/
/***************** CONSTRUCTORS *******************/
/**************************************************/


Physics::Physics()
{
  broadphase = new btDbvtBroadphase();
  collision_config= new btDefaultCollisionConfiguration();
  dispatcher = new btCollisionDispatcher(collision_config);
  solver = new btSequentialImpulseConstraintSolver;
  world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collision_config);
  world->setGravity(btVector3(0,-10,0));
}


Physics::~Physics()
{
  delete world;
  delete solver;
  delete dispatcher;
  delete collision_config;
  delete broadphase;
}


/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/


void Physics::step(Uint32 dt)
{
  world->stepSimulation(1/60.f, 10);
}
