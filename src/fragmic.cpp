#include "fragmic.h"


void Fragmic::toggleDebug()
{
  /*
  static int toggle = 0;

  toggle = !toggle;

  GLuniformBuffer *buffer = buffers.getDebugBuffer();

  std::cout << "TOGGLE: " << toggle << std::endl;

  glm::vec4 d(0.5, 0.5, 0.5, toggle);
  buffer->update(d, 0);
  */
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


/**************************************************/
/***************** CONSTRUCTORS *******************/
/**************************************************/


Fragmic::Fragmic(const std::string &title, const int &width, const int &height):
  camera(width, height), 
  glcontext(), 
  shader(), 
  scene(), 
  window(width, height)
{
  window.init(title);
  if (!glcontext.init(window)) {
    exit(-1);
  }
  shader.load("shaders/animation.v", "shaders/animation.f");
  glcontext.uniform_buffers_init(shader, camera, scene);
  glcontext.swap_interval_set(0);
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

  for (;;) {
    Uint32 timeNow = SDL_GetTicks();
    Uint32 dt = timeNow - lastTime;
    lastTime = timeNow;
    profile_fps(dt);

    if (!window.poll_events(camera, glcontext)) {
      std::cout << "Fragmic exiting..." << std::endl;
      return;
    }
    window.check_error();
    glcontext.clear();
    camera.update((double) dt / 1000.0);

    Node *upload_node = scene.upload_queue_pop();
    while (upload_node) {
      glcontext.vertex_buffers_add(*upload_node);
      upload_node = scene.upload_queue_pop();
    }

    for (auto &node: scene.transform_queue_get()) {
      scene.transform_update(*node, dt);
    }

    Assets &assets = scene.assets_get();
    for (auto &armature: assets.armatures_get()) {
      armature->updateBones();
    }

    glcontext.uniform_buffers_update(shader, camera);
    for (auto &node: scene.render_queue_get()) {
      glcontext.uniform_buffers_update_node(shader, *node);
      glcontext.draw(*node);
    }

    glcontext.swap(window);
  }
}


void Fragmic::term()
{
  glcontext.term();
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




