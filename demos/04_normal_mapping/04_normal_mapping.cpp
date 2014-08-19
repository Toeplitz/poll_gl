#include "fragmic.h"
#include "common.h"
#include <iostream>

Fragmic fragmic;


int main() 
{
  Scene &scene = fragmic.scene_get();
  Physics &physics = fragmic.physics_get();
  GLcontext glcontext = fragmic.glcontext_get();

  {
    Node *camera_node = scene.node_camera_get();
    common_init(fragmic);
    common_fpcamera_use(camera_node);
    common_debug_use();
  }

  Node &crate = scene.load(glcontext, "data/normal_map/", "crate.dae", MODEL_IMPORT_OPTIMIZED);
  physics.collision_shape_add(crate, PHYSICS_COLLISION_TRIANGLE_MESH, true, 1);

  Node &box = scene.load(glcontext, "data/normal_map/", "box_simple.dae", MODEL_IMPORT_OPTIMIZED);
  physics.collision_shape_add(box, PHYSICS_COLLISION_CONVEX_HULL, true, 1);

  Node &floor = scene.load(glcontext, "data/normal_map/", "wood_floor.dae", MODEL_IMPORT_OPTIMIZED);
  physics.collision_shape_add(floor, PHYSICS_COLLISION_CONVEX_HULL, true, 0);

  scene.scene_graph_print(true);
  const Assets &assets = scene.assets_get();
  const Node &root = scene.node_root_get();
  assets.print_all(root);

  fragmic.run();
  fragmic.term();

  return true;
}
