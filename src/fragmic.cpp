#include "fragmic.h"
#include <SDL2/SDL_timer.h> 
#include <iostream>
#include <chrono>


/**************************************************/
/***************** CONSTRUCTORS *******************/
/**************************************************/


Fragmic::Fragmic(const std::string &title, const int &width, const int &height):
  physics(),
  glshader(), 
  glshader_deferred_first(), 
  glshader_deferred_second(), 
  scene(), 
  window(width, height)
{
  GLcontext &glcontext = window.glcontext_get();

  window.init(title);
  window.swap_interval_set(1);
  if (!glcontext.init(window.width, window.height)) {
    exit(-1);
  }

  Node *cam_node = scene.node_create("Camera");
  cam_node->camera_create(scene.assets_get());
  cam_node->camera_get()->transform_perspective_create(window.width, window.height);
  scene.node_camera_set(cam_node);

  /*
  glshader.load("shaders/main.v", "shaders/main.f");
  glcontext.uniform_buffers_create(glshader);
  glshader_screen.load("shaders/post_proc.v", "shaders/post_proc.f");

  Node &node = *scene.node_create("fb_quad");
  node.mesh_create(scene.assets_get());
  node.mesh->quad_generate(1.f);
  glcontext.framebuffer_create(window.width, window.height);
  glcontext.framebuffer_node_create(glshader_screen, node);
*/

  glshader_deferred_first.load("shaders/deferred_pass_one.v", "shaders/deferred_pass_one.f");
  glcontext.uniform_buffers_create(glshader_deferred_first);
  glshader_deferred_second.load("shaders/deferred_pass_two.v", "shaders/deferred_pass_two.f");
  Node &node_g = *scene.node_create("fb_g_quad");
  node_g.mesh_create(scene.assets_get());
  node_g.mesh->quad_generate(1.f);
  glcontext.framebuffer_g_create(glshader_deferred_second, window.width, window.height);
  glcontext.framebuffer_g_node_create(glshader_deferred_second, node_g);

  physics.init();
}


Fragmic::~Fragmic() 
{
}


void Fragmic::draw_g_buffer()
{
  GLcontext &glcontext = window.glcontext_get();

  glshader_deferred_first.use();
  glcontext.framebuffer_g_draw_first_pass(scene, glshader_deferred_first);
  glshader_deferred_second.use();
  glcontext.framebuffer_g_draw_second_pass(glshader_deferred_second);
}


void Fragmic::draw_standard_post_proc(double dt)
{
  GLcontext &glcontext = window.glcontext_get();
  glshader.use();
  glcontext.framebuffer_draw_texture(scene, window.debug);

  /* Step physics simulation */
  physics.step(dt);

  if (!window.debug) {
    glshader_screen.use();
    glcontext.framebuffer_draw_screen();
  }

}


/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/


void Fragmic::run()
{
  GLcontext &glcontext = window.glcontext_get();
  const Assets &assets = scene.assets_get();

  for (;;) {
    double dt = delta_time_get();
    //   profile_fps(dt);

    if (!window.poll_events()) {
      std::cout << "Fragmic exiting..." << std::endl;
      return;
    }

    /* Upload new nodes */
    Node *upload_node = scene.upload_queue_pop();
    while (upload_node) {
      glcontext.vertex_buffers_create(*upload_node);
      upload_node = scene.upload_queue_pop();
    }

    /* Update animations */
    auto &armatures = assets.armature_get_all();
    for (auto &armature: armatures) {
      armature->bones_update_skinningmatrices();
    }
    auto &animated_nodes = scene.animation_list_get();
    for (auto &node: animated_nodes) {
      scene.animation_list_update_transforms(*node, dt);
    }

    /* Update lights */
    unsigned int index = 0;
    auto &lights = assets.light_active_get();
    glcontext.uniform_buffers_update_light_num(lights.size());
    for (auto &light: lights) {
      glcontext.uniform_buffers_update_light(*light, index++);
    }

    /* Update camera */
    Camera &camera = *scene.camera_get();
    camera.update();
    glcontext.uniform_buffers_update_camera(camera);

    /* Draw scene */
 //   draw_standard_post_proc(dt);
    draw_g_buffer();

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
double Fragmic::delta_time_get()
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
