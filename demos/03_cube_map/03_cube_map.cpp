#include "fragmic.h"
#include <iostream>

Fragmic fragmic("Demo 2", 1280, 720);


void keyboard_pressed_cb(SDL_Keysym *keysym)
{
  Physics &physics = fragmic.physics_get();

  switch (keysym->sym) {
    case SDLK_SPACE:
      physics.pause();
      break;
    case SDLK_o:
      physics.debug();
      break;
    default:
      break;
  }
}


int main() 
{
  Scene &scene = fragmic.scene_get();
  Physics &physics = fragmic.physics_get();
  Window &window = fragmic.window_get();
  window.keyboard_pressed_callback_set(keyboard_pressed_cb);

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

  Node &base_node = scene.model_load("data/", "base.dae", MODEL_IMPORT_OPTIMIZED);
  physics.collision_node_add(base_node, PHYSICS_COLLISION_BOX, true, 0);

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
    physics.collision_node_add(node_root, PHYSICS_COLLISION_CONVEX_HULL, true, 1.f);
  }
  physics.pause();

  {

    Node &node = *scene.node_create("skybox");
    node.mesh->cube_generate(150.0f);
    node.material_get()->cubemap_create(CUBEMAP_SKYBOX, "data/game_assets/skybox/SkyboxSet1/DarkStormy/", "DarkStormyFront2048.png",
        "DarkStormyBack2048.png", "bar.png", "foo.png", "DarkStormyLeft2048.png", "DarkStormyRight2048.png");
    scene.upload_queue_add(node);
  }

  scene.scene_graph_print(true);

  fragmic.run();
  fragmic.term();

  return true;
}
