#include "poll.h"
#include "plugin_debug.h"
#include "plugin_light_tool.h"
#include "plugin_firstperson_cam.h"
#include "plugin_node_tool.h"
#include <iostream>



int main() 
{
  Poll poll;
  Scene &scene = poll.scene_get();

  Node *camera_node = scene.node_camera_get();
  auto plugin_debug = std::unique_ptr<Plugin_Debug>(new Plugin_Debug(poll.console_get(), scene));
  auto plugin_light_tool = std::unique_ptr<Plugin_Light_Tool>(new Plugin_Light_Tool(poll.console_get(), scene));
  auto plugin_node_tool = std::unique_ptr<Plugin_Node_Tool>(new Plugin_Node_Tool(poll.console_get(), scene, 0.7f));
  auto plugin_firstperson_camera = std::unique_ptr<Plugin_Firstperson_Camera>(new Plugin_Firstperson_Camera(poll.console_get(), scene, camera_node));
  poll.plugin_add(*plugin_debug);
  poll.plugin_add(*plugin_light_tool);
  poll.plugin_add(*plugin_firstperson_camera);
  poll.plugin_add(*plugin_node_tool);

  const float scene_scalar = 0.7;

  Node &root = scene.node_root_get();
  root.scale(scene, glm::vec3(scene_scalar, scene_scalar, scene_scalar));

 // Node &sponza = scene.load("data/crytek-sponza/", "sponza_with_spec.obj", MODEL_IMPORT_DEFAULT);
 // sponza.scale(scene, glm::vec3(0.05, 0.05, 0.05));
  {
    Node &floor = scene.load("data/demo_room/floor/", "floor.obj", MODEL_IMPORT_OPTIMIZED );
    Material *material = floor.material_get();
    vec3 v(0.5, 0.5, 0.5);
    material->color_set(v, v, v, 1);
  }

  {
    Node &backdrop= scene.load("data/demo_room/backdrop/", "backdrop.dae", MODEL_IMPORT_OPTIMIZED);
    backdrop.rotate(scene, M_PI, vec3(0, 1, 0));
    backdrop.translate(scene, vec3(-25.f * scene_scalar, 0, 0));

    Node &zombie = scene.load("data/zombie/", "new_thin_zombie.dae", MODEL_IMPORT_OPTIMIZED);
    zombie.rotate(scene, (float) M_PI / 2.f, vec3(0, 0, 1));
    zombie.translate(scene, vec3(-18 * scene_scalar, -12.5 * scene_scalar, 9 * scene_scalar));
  }

  {
    Node &backdrop= scene.load("data/demo_room/backdrop/", "backdrop.dae", MODEL_IMPORT_OPTIMIZED);
    backdrop.rotate(scene, M_PI, vec3(0, 1, 0));
    backdrop.translate(scene, vec3(-10.f * scene_scalar, 0, 0));

    Node &teapot= scene.load("data/demo_room/teapot/", "teapot.obj", MODEL_IMPORT_DEFAULT);
    teapot.translate(scene, vec3(-8 * scene_scalar, 2.5 * scene_scalar, 0));
    teapot.scale(scene, vec3(0.02, 0.02, 0.02));

    Node &teapot_no_shadow = scene.load("data/demo_room/teapot/", "teapot.obj", MODEL_IMPORT_DEFAULT);
    teapot_no_shadow.translate(scene, vec3(-12 * scene_scalar, 2.5 * scene_scalar, 0));
    teapot_no_shadow.scale(scene, vec3(0.02, 0.02, 0.02));
    for (auto &child: teapot_no_shadow.children_get()) {
      child->shadow_cast_set(scene, true);
    }
  }

  {
    Node &mitsuba = scene.load("data/demo_room/mitsuba/", "mitsuba.obj", MODEL_IMPORT_DEFAULT);
    mitsuba.translate(scene, vec3(5 * scene_scalar, 0, 0));
  }

  {
    Node &mitsuba = scene.load("data/demo_room/mitsuba/", "mitsuba.obj", MODEL_IMPORT_DEFAULT);
    mitsuba.translate(scene, vec3(20.f * scene_scalar, 0, 0));
  }
 

  /*
  std::vector<std::unique_ptr<Physics_Triangle_Mesh_Shape>> shapes;
  for (auto &child: cube.children_get()) {
    if (!child->mesh_get())
      continue;

    auto shape = std::unique_ptr<Physics_Triangle_Mesh_Shape>(new Physics_Triangle_Mesh_Shape(*child));
    Physics_Rigidbody *rigidbody = child->physics_rigidbody_create(scene);
    if (rigidbody) {
      rigidbody->create(scene.physics_get(), *shape, Physics_Rigidbody::DYNAMIC, 0);
      shapes.push_back(std::move(shape));
    }
  }
  */

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
