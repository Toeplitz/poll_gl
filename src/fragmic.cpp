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
  glshader_geometry(), 
  glshader_light(), 
  scene(), 
  window(width, height)
{
  GLcontext &glcontext = window.glcontext_get();

  window.init(title);
  window.swap_interval_set(1);
  if (!glcontext.init(window.width, window.height)) {
    exit(-1);
  }
  glcontext.check_error();

  Node *cam_node = scene.node_create("camera");
  cam_node->camera_create(scene.assets_get());
  cam_node->camera_get()->transform_perspective_create(window.width, window.height);
  scene.node_camera_set(cam_node);

  glshader_screen.load("shaders/post_proc.v", "shaders/post_proc.f");

  // SETUP FOR DEFERRED SHADING
  glshader_geometry.load("shaders/deferred_pass_one.v", "shaders/deferred_pass_one.f");
  glshader_stencil.load("shaders/stencil_pass.v", "shaders/stencil_pass.f");
  glshader_light.load("shaders/deferred_pass_two.v", "shaders/deferred_pass_two.f");
  glcontext.uniform_locations_geometry_init(glshader_geometry);
  glcontext.uniform_locations_lighting_init(glshader_light);
  glcontext.uniform_buffers_create();
  glcontext.uniform_buffers_block_bind(glshader_geometry);
  glcontext.uniform_buffers_block_bind(glshader_light);
  glcontext.uniform_buffers_block_bind(glshader_stencil);

  /*
  Node &node = *scene.node_create("fb_quad");
  node.mesh_create(scene.assets_get());
  node.mesh->quad_generate(1.f);
  */
  glcontext.framebuffer_create(window.width, window.height);
  //glcontext.framebuffer_node_create(node);

  console.init(scene, glcontext, window);
  physics.init();
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
  const Assets &assets = scene.assets_get();

  for (;;) {
    double dt = delta_time_get();
    profile_fps(dt);

    if (!window.poll_events()) {
      std::cout << "Fragmic exiting..." << std::endl;
      return;
    }

    /* Upload new nodes */
    Node *upload_node = scene.upload_queue_pop();
    while (upload_node) {
      Material *material = upload_node->material_get();
      Mesh *mesh = upload_node->mesh_get();
      Light *light = upload_node->light_get();

      if (light) {
        glcontext.vertex_buffers_light_create(light);
      }

      glcontext.vertex_buffers_mesh_create(mesh);
      glcontext.texture_materials_create(material);
      upload_node = scene.upload_queue_pop();
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
    glcontext.framebuffer_draw_scene(scene.assets_get(), scene, glshader_geometry, glshader_stencil, glshader_light);

    /* Step physics simulation */
    physics.step(dt);

    /* Draw console if toggled */
    console.draw();

    glcontext.check_error();
    window.swap();
  }
}


void Fragmic::term()
{
  GLcontext &glcontext = window.glcontext_get();
  glcontext.uniform_buffers_delete();
  for (auto &node: scene.mesh_nodes_get()) {
    Material *material = node->material_get();
    Mesh *mesh = node->mesh_get();
    glcontext.vertex_buffers_mesh_delete(mesh);
    glcontext.texture_materials_delete(material);
  }
  glshader.term();
  window.term();
}

Console &Fragmic::console_get() 
{
  return console;
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
