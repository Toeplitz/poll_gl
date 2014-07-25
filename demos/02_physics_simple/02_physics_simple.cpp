#include "fragmic.h"
#include "common.h"
#include <iostream>

Fragmic fragmic("Demo 2", 1280, 720);


int main() 
{
  Scene &scene = fragmic.scene_get();
  Assets &assets= scene.assets_get();
  Physics &physics = fragmic.physics_get();

  {
    Node *camera_node = scene.node_camera_get();
    common_init(fragmic);
    common_fpcamera_use(camera_node);
    common_debug_use();
  }


  Node *light_node = scene.node_create("Light");
  Light *light = light_node->light_create(assets);
  light->properties_type_set(LIGHT_DIRECTIONAL);
  light->properties_direction_set(glm::vec3(0, -1, 0));
  light->properties_set(glm::vec3(0.2, 0.2, 0.2), glm::vec3(0.5, 0.5, 0.5), glm::vec3(1.0, 1.0, 1.0));

  Node &box_node_rh = scene.model_load("data/", "box_translated_scaled.dae", MODEL_IMPORT_OPTIMIZED);
  Light *point_light3 = box_node_rh.light_create(assets);
  point_light3->properties_type_set(LIGHT_POINT);
  point_light3->properties_set(glm::vec3(0.2, 0.2, 0.2), glm::vec3(1.0, 1.0, 1.0), glm::vec3(1.0, 1.0, 1.0));
  point_light3->bias_set(glm::vec3(0, 1.2, 0));
  physics.collision_shape_add(box_node_rh, PHYSICS_COLLISION_BOX, true, 1.f);

  Node &sphere_node = scene.model_load("data/", "sphere_translated_scaled.dae", MODEL_IMPORT_OPTIMIZED);
  Light *point_light = sphere_node.light_create(assets);
  point_light->properties_type_set(LIGHT_POINT);
  point_light->properties_set(glm::vec3(0.2, 0.2, 0.2), glm::vec3(1.0, 1.0, 1.0), glm::vec3(1.0, 1.0, 1.0));
  point_light->bias_set(glm::vec3(0, 1.2, 0));

  physics.collision_shape_add(sphere_node, PHYSICS_COLLISION_SPHERE, true, 1.f);

  Node &monkey_node = scene.model_load("data/", "convex_hull.dae", MODEL_IMPORT_OPTIMIZED);
  Light *point_light2 = monkey_node.light_create(assets);
  point_light2->properties_type_set(LIGHT_POINT);
  point_light2->properties_set(glm::vec3(0.2, 0.2, 0.2), glm::vec3(1.0, 1.0, 1.0), glm::vec3(1.0, 1.0, 1.0));
  point_light2->bias_set(glm::vec3(0, 1.2, 0));
  physics.collision_shape_add(monkey_node, PHYSICS_COLLISION_CONVEX_HULL, true, 1.f);

  Node &base_node = scene.model_load("data/", "base.dae", MODEL_IMPORT_OPTIMIZED);
  physics.collision_shape_add(base_node, PHYSICS_COLLISION_BOX, true, 0);

  physics.pause();
  scene.scene_graph_print(true);

  fragmic.run();
  fragmic.term();

  return true;
}
