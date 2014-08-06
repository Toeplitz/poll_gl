#include "fragmic.h"
#include "common.h"
#include <iostream>

Fragmic fragmic("Demo 2", 1280, 720);


int main() 
{
  Scene &scene = fragmic.scene_get();
  Assets &assets = scene.assets_get();
  Physics &physics = fragmic.physics_get();

  {
    Node *camera_node = scene.node_camera_get();
    common_init(fragmic);
    common_fpcamera_use(camera_node);
    common_debug_use();
  }

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
 
  /*
  Node *light_node = scene.node_create("Light");
  Light *light = light_node->light_create(assets);
  light->properties_type_set(LIGHT_DIRECTIONAL);
  light->properties_direction_set(glm::vec3(0, -1, 0));
  light->properties_set(glm::vec3(0.2, 0.2, 0.2), glm::vec3(1, 1, 1), glm::vec3(1.0, 1.0, 1.0));
  
  Node &base_node = scene.model_load("data/", "base.dae", MODEL_IMPORT_OPTIMIZED);
  physics.collision_shape_add(base_node, PHYSICS_COLLISION_BOX, true, 0);

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
    physics.collision_shape_add(node_root, PHYSICS_COLLISION_CONVEX_HULL, true, 1.f);
    Light *point_light = cube.light_create(assets);
    point_light->properties_type_set(LIGHT_POINT);
    point_light->properties_set(glm::vec3(0.2, 0.2, 0.2), glm::vec3(1.0, 1.0, 1.0), glm::vec3(1.0, 1.0, 1.0));
    point_light->bias_set(glm::vec3(-1, 2.2, -1));
  }
  physics.pause();

  {

    Node &node = *scene.node_create("skybox");
    Mesh *mesh = node.mesh_create(assets);
    Material *material = node.material_create(assets);
    mesh->cube_generate(150.0f);
    material->cubemap_create(CUBEMAP_SKYBOX, "data/game_assets/skybox/SkyboxSet1/DarkStormy/", "DarkStormyFront2048.png",
        "DarkStormyBack2048.png", "bar.png", "foo.png", "DarkStormyLeft2048.png", "DarkStormyRight2048.png");
    scene.upload_queue_add(node);
  }
  */

  scene.scene_graph_print(true);

  fragmic.run();
  fragmic.term();

  return true;
}
