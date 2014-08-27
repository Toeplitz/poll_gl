#include "poll.h"
#include <iostream>
#include <chrono>


/**************************************************/
/***************** CONSTRUCTORS *******************/
/**************************************************/


Poll::Poll(const std::string &config_file)
{
  GLcontext &glcontext = window.glcontext_get();
  Assets &assets = assets_get();

  if (config_file.empty()) 
    config.init(console, scene, glcontext, GLOBAL_CONFIG);
  else
    config.init(console, scene, glcontext, config_file);

  window.init(config, "Poll");
  window.swap_interval_set(1);
  if (!glcontext.init(window.width_get(), window.height_get())) {
    exit(-1);
  }
  glcontext.check_error();

  assets.init(glcontext, scene);

  Node *cam_node = scene.node_create("camera");
  cam_node->camera_create(scene.assets_get());
  cam_node->camera_get()->transform_perspective_create(window.width_get(), window.height_get());
  scene.node_camera_set(cam_node);

  glshader_geometry.load("shaders/deferred_pass_one.v", "shaders/deferred_pass_one.f");
  glshader_light.load("shaders/deferred_pass_two.v", "shaders/deferred_pass_two.f");
  glshader_post_proc.load("shaders/post_proc.v", "shaders/post_proc.f");
  glshader_stencil.load("shaders/stencil_pass.v", "shaders/stencil_pass.f");
  glshader_text.load("shaders/text.v", "shaders/text.f"); 
  glcontext.uniform_locations_geometry_init(glshader_geometry);
  glcontext.uniform_locations_lighting_init(glshader_light);
  glcontext.uniform_locations_post_proc_init(glshader_post_proc);
  glcontext.uniform_locations_text_init(glshader_text);
  glcontext.uniform_buffers_create(config);
  glcontext.uniform_buffers_block_bind(glshader_geometry);
  glcontext.uniform_buffers_block_bind(glshader_light);
  glcontext.uniform_buffers_block_bind(glshader_stencil);

  glcontext.framebuffer_create(window.width_get(), window.height_get());

  console.init(glcontext, scene, window);
  ui.init(console, glcontext, scene);
  physics.init();

  config.conf_global_apply(CONF_GLOBAL);
}


/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/


void Poll::run()
{
  GLcontext &glcontext = window.glcontext_get();
  const Assets &assets = assets_get();

  for (;;) {
    double dt = delta_time_get();
    profile_fps(dt);

    if (!window.poll_events()) {
      std::cout << "Poll exiting..." << std::endl;
      return;
    }

    /* Update animations */
    auto &animated_nodes = scene.animated_nodes_get();
    for (auto &node: animated_nodes) {
      scene.animated_nodes_update_transforms(*node, dt);
    }
    auto &armatures = assets.armature_get_all();
    for (auto &armature: armatures) {
      armature->bones_update_skinningmatrices();
      glcontext.uniform_buffers_update_armature(*armature);
    }

    /* Update camera */
    Camera &camera = *scene.camera_get();
    camera.update();
    glcontext.uniform_buffers_update_camera(camera);

    /* Draw scene */
    glcontext.framebuffer_draw_scene(scene, glshader_geometry, glshader_stencil, glshader_light, glshader_post_proc);

    /* Step physics simulation */
    physics.step(dt);

    /* Draw console and ui */
    glshader_text.use();
    console.draw();
    ui.draw(fps_text_get());

    glcontext.check_error();
    window.swap();
  }
}


void Poll::term()
{
  Assets &assets = assets_get();
  GLcontext &glcontext = window.glcontext_get();

  console.term();
  physics.term();
  assets.term(glcontext);
  glcontext.uniform_buffers_delete();
  glshader_geometry.term();
  glshader_light.term();
  glshader_stencil.term();
  glshader_text.term();
  window.term();
}


Assets &Poll::assets_get()
{
  return scene_get().assets_get();
}


Config &Poll::config_get() 
{
  return config;
}


Console &Poll::console_get() 
{
  return console;
}


GLcontext &Poll::glcontext_get()
{
  return window_get().glcontext_get();
}


Physics &Poll::physics_get() 
{
  return physics;
}

Scene &Poll::scene_get()
{
  return scene;
}


Window &Poll::window_get()
{
  return window;
}


/**************************************************/
/***************** PRIVATE METHODS ****************/
/**************************************************/


// Return delta time in seconds.
double Poll::delta_time_get()
{
  static unsigned long long time_last = std::chrono::system_clock::now().time_since_epoch() /  std::chrono::microseconds(1);
  unsigned long long time_cur = std::chrono::system_clock::now().time_since_epoch() /  std::chrono::microseconds(1);
  double dt = (time_cur - time_last) / 1000.0 / 1000.0;
  time_last = time_cur;

  return dt;
}

std::string &Poll::fps_text_get()
{
  return fps_text;
}


void Poll::profile_fps(const double dt)
{
  static int numFrames = 0;
  static double t;

  numFrames++;
  t += dt; 

  if (t >= 1) {
    char buf[30];
    sprintf(buf, "%d frames/sec %.2f ms/frame", numFrames, 1000.0 / numFrames);
    fps_text = std::string(buf);
    t = 0;
    numFrames = 0;
  }
}
