#include "fragmic.h"
#include <iostream>


void collision_cb(int i)
{
  std::cout << "Detected collision" << std::endl;
}



int main() 
{
  Fragmic fragmic("Demo 2", 1280, 720);
  Scene &scene = fragmic.scene_get();
  Physics &physics = fragmic.physics_get();

  Node &node = scene.load_model("data/", "scene_textured.dae");
  //physics.collision_node_add(node, PHYSICS_COLLISION_SPHERE, true);
  //physics.collision_node_callback_set(node, collision_cb);

  scene.load_model("data/", "static_unit_box.dae");
  scene.scene_graph_print();

  fragmic.run();
  fragmic.term();

  return true;
}
