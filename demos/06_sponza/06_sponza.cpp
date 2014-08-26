#include "poll.h"
#include "common.h"
#include <iostream>

Poll poll;


int main() 
{
  Scene &scene = poll.scene_get();
  GLcontext glcontext = poll.glcontext_get();

  {
    Node *camera_node = scene.node_camera_get();
    common_init(poll);
    common_fpcamera_use(camera_node);
    common_debug_use();
  }

  Node &root = scene.node_root_get();
  Node &sponza = scene.load(glcontext, "data/crytek-sponza/", "sponza_with_spec.obj", MODEL_IMPORT_OPTIMIZED);
  root.scale(glm::vec3(0.01, 0.01, 0.01));
  // scene.load("data/crytek-sponza/", "banner.obj", MODEL_IMPORT_OPTIMIZED);

  scene.scene_graph_print(false);
  //scene.assets_get().print_all(scene.node_root_get());


  poll.run();
  poll.term();

  return true;
}
