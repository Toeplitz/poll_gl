#include "fragmic.h"
#include <SDL2/SDL_timer.h>             // for SDL_GetTicks
#include <iostream>                     // for operator<<, basic_ostream, etc


/**************************************************/
/***************** CONSTRUCTORS *******************/
/**************************************************/


Fragmic::Fragmic(const std::string &title, const int &width, const int &height):
  camera(width, height), 
  shader(), 
  scene(), 
  window(width, height)
{
  window.init(title);
  window.swap_interval_set(0);
  if (!window.glcontext_get().init(window.width, window.height)) {
    exit(-1);
  }
  shader.load("shaders/animation.v", "shaders/animation.f");
  window.glcontext_get().uniform_buffers_init(shader);
}


Fragmic::~Fragmic() 
{
}


/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/


void Fragmic::run()
{
  Uint32 lastTime = SDL_GetTicks();
  GLcontext &glcontext = window.glcontext_get();
  Assets &assets = scene.assets_get();

  for (;;) {
    Uint32 timeNow = SDL_GetTicks();
    Uint32 dt = timeNow - lastTime;
    lastTime = timeNow;
    profile_fps(dt);

    if (!window.poll_events(camera)) {
      std::cout << "Fragmic exiting..." << std::endl;
      return;
    }

    Node *upload_node = scene.upload_queue_pop();
    while (upload_node) {
      glcontext.vertex_buffers_add(*upload_node);
      upload_node = scene.upload_queue_pop();
    }

    for (auto &armature: assets.armature_get_all()) {
      armature->bones_update_skinningmatrices();
    }
    for (auto &node: scene.animation_list_get()) {
      scene.animation_list_update_transforms(*node, dt);
    }

    camera.update((double) dt / 1000.0);
    glcontext.uniform_buffers_update_camera(camera);
    glcontext.clear();
    for (auto &node: scene.render_list_get()) {
      glcontext.draw(*node, window.aabb_view_toggle);
    }
    glcontext.check_error();
    window.swap();
  }
}


void Fragmic::term()
{
  window.term();
}


Camera &Fragmic::camera_get() 
{
  return camera;
}


Scene &Fragmic::scene_get()
{
  return scene;
}


Window &Fragmic::window_get()
{
  return window;
}




/**************************************************/
/***************** PRIVATE METHODS ****************/
/**************************************************/


void Fragmic::profile_fps(Uint32 dt)
{
  static int numFrames = 0;
  static Uint32 t;

  numFrames++;
  t += dt;

  if (t >= 1000) {
    std::cout << numFrames << " frames/sec, " << 1000.0 /
      numFrames << " ms/frame" << std::endl;
    t = 0;
    numFrames = 0;
  }
}
