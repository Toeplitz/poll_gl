#include "poll.h"
#include "poll_plugin.h"
#include "scene.h"
#include <iostream>
#include <chrono>


/**************************************************/
/***************** CONSTRUCTORS *******************/
/**************************************************/


Poll::Poll(const std::string &config_file)
{
  // GLcontext &glcontext = glcontext_get();
  // Physics &physics = physics_get();
  // Assets &assets = assets_get();

  // if (config_file.empty())
  //   config.init(console, scene, GLOBAL_CONFIG);
  // else
  //   config.init(console, scene, config_file);

  //window.init(config, scene, GLOBAL_TITLE);
  // glcontext.init();
  // scene.init(plugins);
  // assets.init(scene);
  //console.init(scene, window);
  // ui.init(console, glcontext, scene);
  // physics.init();

  // config.conf_global_apply(CONF_GLOBAL);
}


/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/

// Return delta time in seconds.
void Poll::delta_time_step()
{
  static auto time_last = std::chrono::duration_cast<std::chrono::milliseconds>
        (std::chrono::system_clock::now().time_since_epoch()).count();
  auto time_cur = std::chrono::duration_cast<std::chrono::milliseconds>
        (std::chrono::system_clock::now().time_since_epoch()).count();

  this->dt = (time_cur - time_last) / 1000.0;
  time_last = time_cur;
}


std::string &Poll::fps_text_get()
{
  return fps_text;
}


void Poll::init()
{
  glcontext.init();
}



void Poll::step(Scene &scene)
{
  delta_time_step();
  double dt = delta_time_get();

  profile_fps(dt);

  for (auto plugin : plugins) {
    plugin->cb_custom(dt);
  }

  /* Update animations */
  auto &animated_nodes = scene.animated_nodes_get();
  for (auto &node: animated_nodes) {
    scene.animated_nodes_update_transforms(*node, dt);
  }
  auto &armatures = scene.assets_get().armature_get_all();
  for (auto &armature: armatures) {
    armature->bones_update_skinningmatrices();
  }

  /* Update camera */
  Camera &camera = *scene.camera_get();
  glcontext.uniform_buffers_update_camera(camera);

  /* Draw scene */
  glcontext.draw_scene(scene, plugins);

  /* Draw console and ui */
  // shader.text.use();
  // console.draw();
  // ui.draw(fps_text_get());

  glcontext.check_error();
}


void Poll::term()
{

  glcontext.term();
}


GLcontext &Poll::glcontext_get()
{
  return glcontext;
}


void Poll::plugin_add(Poll_Plugin &plugin)
{
  plugins.push_back(&plugin);
}


const Poll_Plugin_List &Poll::plugins_get() const
{
  return plugins;
}


/**************************************************/
/***************** PRIVATE METHODS ****************/
/**************************************************/


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


