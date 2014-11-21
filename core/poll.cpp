#include "poll.h"
#include "poll_plugin.h"
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
double Poll::delta_time_get()
{
 // double dt = 0.01;
  static unsigned long long time_last = std::chrono::system_clock::now().time_since_epoch() /  std::chrono::microseconds(1);
  unsigned long long time_cur = std::chrono::system_clock::now().time_since_epoch() /  std::chrono::microseconds(1);
  double dt = (time_cur - time_last) / 1000.0 / 1000.0;
  time_last = time_cur;

  return dt;
}

void Poll::init()
{
  glcontext.init();
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
    POLL_DEBUG(std::cout, fps_text);
    t = 0;
    numFrames = 0;
  }
}

void Poll::run()
{
 // GLcontext &glcontext = glcontext_get();
 // Assets &assets = assets_get();
  //Stock_Shaders &shader = assets.stock_shaders_get();

#if 0
  for (;;) {
    double dt = delta_time_get();
    profile_fps(dt);

    //if (!window.poll_events(plugins))
    //  return;

    for (auto plugin : plugins) {
      plugin->cb_custom(dt);
    }

    /* Update animations */
    auto &animated_nodes = scene.animated_nodes_get();
    for (auto &node: animated_nodes) {
      scene.animated_nodes_update_transforms(*node, dt);
    }
    auto &armatures = assets.armature_get_all();
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
   // window.swap();
  }
#endif
}


void Poll::term()
{
 // Assets &assets = assets_get();
  //GLcontext &glcontext = window.glcontext_get();
 // Physics &physics = physics_get();

  //console.term();
  //physics.term();
  //assets.term(scene);
  glcontext.term();
  //window.term();
}


//Assets &Poll::assets_get()
//{
//  return scene_get().assets_get();
//}



Console &Poll::console_get() 
{
  return console;
}


GLcontext &Poll::glcontext_get()
{
  return glcontext;
}


//Physics &Poll::physics_get() 
//{
 // return scene.physics_get();
//}


void Poll::plugin_add(Poll_Plugin &plugin)
{
  plugins.push_back(&plugin);
}


/**************************************************/
/***************** PRIVATE METHODS ****************/
/**************************************************/



std::string &Poll::fps_text_get()
{
  return fps_text;
}

