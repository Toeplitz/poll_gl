#include "fragmic.h"
#include <iostream>

Fragmic fragmic("Demo 2", 1280, 720);


int main() 
{
  Scene &scene = fragmic.scene_get();

  /*
  {
    Node &node = scene.model_load("data/game_assets/", "box.dae");
  }
  */

  /*
  {
    Node &node = *scene.node_create("box");
    node.mesh->cube_generate(3.0f);
    node.material->color_set(glm::vec3(1.f, 0.f, 0.f), 
        glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 0.f, 1.f), 40.f);
    scene.upload_queue_add(node);
  }
  */

  std::unique_ptr<Material> material(new Material());
  material->cubemap_create(CUBEMAP_REFLECTION, "data/game_assets/skybox/SkyboxSet1/DarkStormy/", "DarkStormyFront2048.png",
    "DarkStormyBack2048.png", "bar.png", "foo.png", "DarkStormyLeft2048.png", "DarkStormyRight2048.png");

  {
    Node &node_root = scene.model_load("data/game_assets/", "sphere.dae");
    Node &sphere = *scene.node_find(&node_root, "Sphere");
    Node &cube = *scene.node_find(&node_root, "Cube");
    Node &suzanne= *scene.node_find(&node_root, "Suzanne");
    cube.material_set(material.get());
    sphere.material_set(material.get());
    suzanne.material_set(material.get());
    scene.state_update_recursive(sphere);
    scene.state_update_recursive(cube);
    scene.state_update_recursive(suzanne);
  }

  {

    Node &node = *scene.node_create("skybox");
    node.mesh->cube_generate(150.0f);
    node.material_get()->cubemap_create(CUBEMAP_SKYBOX, "data/game_assets/skybox/SkyboxSet1/DarkStormy/", "DarkStormyFront2048.png",
        "DarkStormyBack2048.png", "bar.png", "foo.png", "DarkStormyLeft2048.png", "DarkStormyRight2048.png");
    scene.upload_queue_add(node);
  }

  scene.scene_graph_print(true);
  scene.assets_get().print_all(scene.node_root_get());

  fragmic.run();
  fragmic.term();

  return true;
}
