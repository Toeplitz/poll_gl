#include "fragmic.h"
#include <SDL2/SDL_timer.h> 
#include <iostream>
#include <chrono>

/* TODO:
 * 
 * For optimization:
 * - Remove if statements in glshader, espacially for weights.
 * - Frustnum culling
 *   - Calculate AABB, minimum/maximum vertex approach.
 */

/**************************************************/
/***************** CONSTRUCTORS *******************/
/**************************************************/


Fragmic::Fragmic(const std::string &title, const int &width, const int &height):
  camera(width, height), 
  physics(),
  glshader(), 
  scene(), 
  window(width, height)
{
  GLcontext &glcontext = window.glcontext_get();

  window.init(title);
  window.swap_interval_set(1);
  if (!glcontext.init(window.width, window.height)) {
    exit(-1);
  }

  glshader.load("shaders/main.v", "shaders/main.f");
  glshader.print_block_names();
  glcontext.uniform_buffers_create(glshader);
  physics.init();

  // Setup quad for framebuffers.
  glshader_screen.load("shaders/post_proc.v", "shaders/post_proc.f");

  Node &node = *scene.node_create_mesh_only("fb_quad");
  node.mesh->quad_generate(1.f);
  glcontext.framebuffer_node_set(node);
  glcontext.vertex_buffers_create(node);

}


Fragmic::~Fragmic() 
{
}


/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/


void Fragmic::run()
{
  GLcontext &glcontext = window.glcontext_get();
  Assets &assets = scene.assets_get();

  for (;;) {
    double dt = delta_time_get();
 //   profile_fps(dt);

    if (!window.poll_events(camera)) {
      std::cout << "Fragmic exiting..." << std::endl;
      return;
    }

    Node *upload_node = scene.upload_queue_pop();
    while (upload_node) {
      glcontext.vertex_buffers_create(*upload_node);
      upload_node = scene.upload_queue_pop();
    }
    for (auto &armature: assets.armature_get_all()) {
      armature->bones_update_skinningmatrices();
    }
    for (auto &node: scene.animation_list_get()) {
      scene.animation_list_update_transforms(*node, dt);
    }

    glcontext.clear();
    physics.step(dt);

    glshader.use();
    camera.update(dt);
    glcontext.uniform_buffers_update_camera(camera);
    glcontext.framebuffer_draw_texture(scene);

    glshader_screen.use();
    glcontext.framebuffer_draw_screen();

    glcontext.check_error();
    window.swap();
  }
}


void Fragmic::term()
{
  GLcontext &glcontext = window.glcontext_get();
  glcontext.uniform_buffers_delete();
  for (auto &node: scene.render_list_get()) {
    glcontext.vertex_buffers_delete(*node);
  }
  glshader.term();
  window.term();
}


Camera &Fragmic::camera_get() 
{
  return camera;
}

Physics &Fragmic::physics_get() 
{
  return physics;
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


// Return delta time in seconds.
const double Fragmic::delta_time_get()
{
  static unsigned long long time_last = std::chrono::system_clock::now().time_since_epoch() /  std::chrono::microseconds(1);
  unsigned long long time_cur = std::chrono::system_clock::now().time_since_epoch() /  std::chrono::microseconds(1);
  double dt = (time_cur - time_last) / 1000.0 / 1000.0;
  time_last = time_cur;

  return dt;
}


void Fragmic::profile_fps(const double dt)
{
  static int numFrames = 0;
  static double t;

  numFrames++;
  t += dt; 

  if (t >= 1) {
    std::cout << numFrames << " frames/sec, " << 1000.0 /
      numFrames << " ms/frame" << std::endl;
    t = 0;
    numFrames = 0;
  }
}
