#include "poll.h"
#include "plugin_debug.h"
#include "plugin_firstperson_cam.h"
#include <iostream>



int main() 
{
  Poll poll;
  Scene &scene = poll.scene_get();

  Node *camera_node = scene.node_camera_get();
  auto firstperson_camera = std::unique_ptr<Plugin_Firstperson_Camera>(new Plugin_Firstperson_Camera(poll.console_get(), scene, camera_node));
  poll.plugin_add(*firstperson_camera);

  auto debug = std::unique_ptr<Plugin_Debug>(new Plugin_Debug(poll.console_get(), scene));
  poll.plugin_add(*debug);

  Node &sponza = scene.load("data/crytek-sponza/", "sponza_with_spec.obj", MODEL_IMPORT_DEFAULT);
  sponza.scale(scene, glm::vec3(0.1, 0.1, 0.1));

  std::vector<std::unique_ptr<Physics_Triangle_Mesh_Shape>> shapes;
  for (auto &child: sponza.children_get()) {
    if (!child->mesh_get())
      continue;

    auto shape = std::unique_ptr<Physics_Triangle_Mesh_Shape>(new Physics_Triangle_Mesh_Shape(*child));
    Physics_Rigidbody *rigidbody = child->physics_rigidbody_create(scene);
    if (rigidbody) {
      rigidbody->create(scene.physics_get(), *shape, Physics_Rigidbody::DYNAMIC, 0);
      shapes.push_back(std::move(shape));
    }
  }

  Node *foo = &scene.load("data/", "cone.dae", MODEL_IMPORT_OPTIMIZED);
  foo->translate(scene, glm::vec3(0, 0, 40));
 // foo->physics_rigidbody_create(scene, false, Physics_Rigidbody::CONVEX_HULL, Physics_Rigidbody::DYNAMIC, 1.f);

  {
    Node *node = scene.node_create("Light_Directionl_Global");
    Light *light = node->light_create(scene, Light::DIRECTIONAL, Light::GLOBAL);
    node->translate(scene, glm::vec3(0, 40, 0));
    light->properties_direction_set(glm::vec3(0, -1, -1));
    light->properties_color_set(glm::vec3(0.5, 0.5, 0.5));
  }

  poll.run();
  poll.term();

  return true;
}
