#include "plugin_node_tool.h"
#include "physics_rigidbody.h"
#include <glm/gtx/string_cast.hpp>

/**************************************************/
/***************** PRIVATE METHODS ****************/
/**************************************************/


void Plugin_Node_Tool::relative_position_set(Node *node, vec3 hitpoint)
{
  if (node->grab_parent) {
    relative_position = node->parent_get()->position_local_get() - hitpoint;
  } else {
    relative_position = node->position_local_get() - hitpoint;
  }
}


/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/


Plugin_Node_Tool::Plugin_Node_Tool(Console &console, Scene &scene, float gizmo_zoom_factor = 1.f)
{
  GLcontext &glcontext = scene.glcontext_get();

  this->console = &console;
  this->scene = &scene;

  keypress_map[SDLK_x] = std::make_pair(false, "maniplation on x-axis");
  keypress_map[SDLK_y] = std::make_pair(false, "maniplation on y-axis");
  keypress_map[SDLK_z] = std::make_pair(false, "maniplation on z-axis");
  keypress_map[SDLK_t] = std::make_pair(false, "translate mode");
  keypress_map[SDLK_g] = std::make_pair(false, "scale mode");
  keypress_map[SDLK_LSHIFT] = std::make_pair(false, "shift");

  this->gizmo_zoom_factor = gizmo_zoom_factor;
  node_gizmo = &scene.load("data/", "gizmo_translate.dae", MODEL_IMPORT_OPTIMIZED  | MODEL_IMPORT_NO_DRAW);
  for (auto &child: node_gizmo->children_get()) {
    child->grab_parent = true;
    auto shape = std::unique_ptr<Physics_Triangle_Mesh_Shape>(new Physics_Triangle_Mesh_Shape(*child));
    Physics_Rigidbody *rigidbody = child->physics_rigidbody_create(scene);
    if (rigidbody) {
      rigidbody->create(scene.physics_get(), *shape, Physics_Rigidbody::KINEMATIC, 0);
    }
    shapes.push_back(std::move(shape));

  }

  node_gizmo_translate_x = scene.node_find(node_gizmo, "x");
  node_gizmo_translate_y = scene.node_find(node_gizmo, "y");
  node_gizmo_translate_z = scene.node_find(node_gizmo, "z");
  node_gizmo_translate_x->raycast_priority_set(1);
  node_gizmo_translate_y->raycast_priority_set(1);
  node_gizmo_translate_z->raycast_priority_set(1);

  material_red.color_set(vec3(0, 0, 0), vec3(1, 0, 0), vec3(0, 0, 0), 100);
  material_green.color_set(vec3(0, 0, 0), vec3(0, 1, 0), vec3(0, 0, 0), 100);
  material_blue.color_set(vec3(0, 0, 0), vec3(0, 0, 1), vec3(0, 0, 0), 100);
  node_gizmo_translate_x->material_set(&material_red);
  node_gizmo_translate_y->material_set(&material_green);
  node_gizmo_translate_z->material_set(&material_blue);

  node_bounding_box = scene.node_create("bounding_box");
  Mesh *mesh = node_bounding_box->mesh_create(scene);
  mesh->generate_line_cube(1.f);
  glcontext.vertex_buffers_mesh_create(mesh);
}


void Plugin_Node_Tool::cb_node_draw(Node &node)
{
  GLcontext &glcontext = scene->glcontext_get();
  Physics_Rigidbody *rigidbody = node.physics_rigidbody_get();
  Camera &camera = *scene->camera_get();

  if (!rigidbody) {
    return;
  }

  /* DRAW SELECTION OUTLINE */
  Node *node_outline = node_bounding_box;
  Aabb &aabb = node.aabb_get();

  node_outline->transform_global_from_node_set(node, glm::scale(glm::mat4(1.f), aabb.r / node.scale_global_get()));
  glcontext.uniform_buffers_update_matrices(*node_outline);
  glLineWidth(2.f);
  glcontext.draw_mesh(*node_outline);


  /* DRAW GIZMO */
  vec3 pos = node.position_get();
  float len = glm::length(camera.target_position_get() - pos);

  vec3 v = vec3(len, len, len) * gizmo_zoom_factor;
  node_gizmo->scale_identity(*scene, v);
  node_gizmo->translate_identity(*scene, pos);

  glcontext.uniform_buffers_update_matrices(*node_gizmo_translate_x);
  glcontext.uniform_buffers_update_material(*node_gizmo_translate_x->material_get());
  glcontext.draw_mesh(*node_gizmo_translate_x);

  glcontext.uniform_buffers_update_matrices(*node_gizmo_translate_y);
  glcontext.uniform_buffers_update_material(*node_gizmo_translate_y->material_get());
  glcontext.draw_mesh(*node_gizmo_translate_y);

  glcontext.uniform_buffers_update_matrices(*node_gizmo_translate_z);
  glcontext.uniform_buffers_update_material(*node_gizmo_translate_z->material_get());
  glcontext.draw_mesh(*node_gizmo_translate_z);
}


void Plugin_Node_Tool::cb_keyboard_pressed(SDL_Keysym *keysym)
{
  if (keypress_map.count(keysym->sym) == 0)
    return;

  keypress_map[keysym->sym].first = !keypress_map[keysym->sym].first;

  if (keypress_map[keysym->sym].first) {
    std::cout << keypress_map[keysym->sym].second << ": " << "enabled" << std::endl;
  } else {
    std::cout << keypress_map[keysym->sym].second << ": " << "disabled" << std::endl;
  }
}


void Plugin_Node_Tool::cb_keyboard_released(SDL_Keysym *keysym)
{
}


Node *Plugin_Node_Tool::hitpoint_last_node_get()
{
  if (hitpoints.size() <= 0)
    return nullptr;

  auto &hitpoint_last = *hitpoints.begin();
  return hitpoint_last.node_ptr;
}


Raycast_Hitpoint *Plugin_Node_Tool::hitpoint_last_get()
{
  if (hitpoints.size() <= 0)
    return nullptr;

  auto &hitpoint_last = *hitpoints.begin();
  return &hitpoint_last;
}



void Plugin_Node_Tool::cb_mouse_pressed(SDL_MouseButtonEvent *ev)
{
  auto height = scene->window_get().height_get();
  auto width = scene->window_get().width_get();
  auto hp = raycast.cast(*scene, ev->x, ev->y, width, height);

  if (ev->button != SDL_BUTTON_LEFT)
    return;

  if (!hp.length)
    return;

  Node *node_current = hp.node_ptr;
  Node *node_last = hitpoint_last_node_get();
  hitpoint_world = hp.world_hitpoint;
  ray_from = hp.ray_from;

  if (node_current == node_last) return;

  Physics_Rigidbody *rb = node_current->physics_rigidbody_get();
  if (rb->type_get() != Physics_Rigidbody::KINEMATIC)
    return;

  POLL_DEBUG(std::cout, "node current: " << node_current->name_get());

  if (node_last)
    POLL_DEBUG(std::cout, "node last: " << node_last->name_get());

  if (node_current == node_gizmo_translate_x) {
    keypress_map[SDLK_y].first = false;
    keypress_map[SDLK_z].first = false;
    keypress_map[SDLK_x].first = true;
    keypress_map[SDLK_t].first = true;
    node_motion = node_gizmo_translate_x;
    relative_position_set(node_last, hitpoint_world);
    node_gizmo_translate_x->link_set(node_last);
    this->mouse_down = true;
    return;
  } 
  if (node_current == node_gizmo_translate_y) {
    keypress_map[SDLK_x].first = false;
    keypress_map[SDLK_z].first = false;
    keypress_map[SDLK_y].first = true;
    keypress_map[SDLK_t].first = true;
    node_motion = node_gizmo_translate_y;
    relative_position_set(node_last, hitpoint_world);
    node_gizmo_translate_y->link_set(node_last);
    this->mouse_down = true;
    return;
  } 
  if (node_current == node_gizmo_translate_z) {
    keypress_map[SDLK_x].first = false;
    keypress_map[SDLK_y].first = false;
    keypress_map[SDLK_z].first = true;
    keypress_map[SDLK_t].first = true;
    node_motion = node_gizmo_translate_z;
    relative_position_set(node_last, hitpoint_world);
    node_gizmo_translate_z->link_set(node_last);
    this->mouse_down = true;
    return;
  } 

  node_current->callback_draw_set(std::bind(&Plugin_Node_Tool::cb_node_draw, this, _1));

  if (node_last) {
    node_last->callback_draw_set(nullptr);
    hitpoints.pop_back();
  }

  hitpoints.push_back(hp);
}


void Plugin_Node_Tool::cb_mouse_released(SDL_MouseButtonEvent *ev)
{
  this->mouse_down = false;
}


void Plugin_Node_Tool::cb_mouse_motion(SDL_MouseMotionEvent *ev)
{
  if (!mouse_down)
    return;

  Node *node = node_motion;
  if (!node) {
    POLL_ERROR(std::cerr, "No node attached to the last hipoint.");
    return;
  }

  Node *node_link = node->link_get();
  if (!node_link) {
    POLL_ERROR(std::cerr, "No link node attached to the object.");
    return;
  }

  Physics_Rigidbody *rb = node_link->physics_rigidbody_get();
  if (!rb) {
    POLL_ERROR(std::cerr, "No rigidbody on object which was clicked??");
    return;
  }

  auto height = scene->window_get().height_get();
  auto width = scene->window_get().width_get();
  auto world_ray = raycast.get_ray_to(*scene, ev->x, ev->y, width, height);

  btVector3 newRayTo = btVector3(world_ray.x, world_ray.y, world_ray.z);
  btVector3 rayFrom;

  vec3 camera_pos = scene->camera_get()->position_get();
  float dist = glm::length(hitpoint_world - ray_from);

  rayFrom = btVector3(camera_pos.x, camera_pos.y, camera_pos.z);
  btVector3 dir = newRayTo - rayFrom;
  dir.normalize();
  dir *= dist;

  btVector3 newPivotB = rayFrom + dir;

  if (rb->type_get() == Physics_Rigidbody::KINEMATIC) {
    Node *ptr = node_motion;
    if (node_motion->grab_parent) {
      ptr = node_motion->parent_get();
    }

    if (keypress_map[SDLK_t].first) {
      vec3 move_to = vec3(newPivotB.getX(), newPivotB.getY(), newPivotB.getZ());

      vec3 pos;
      if (node_link->grab_parent) {
        pos = node_link->parent_get()->position_local_get();
      } else {
        pos = node_link->position_local_get();
      }

      vec3 new_pos(pos.x, pos.y, pos.z);
      if (keypress_map[SDLK_x].first) {
        new_pos.x = (move_to.x + relative_position.x);
      } 
      if (keypress_map[SDLK_y].first) {
        new_pos.y = move_to.y + relative_position.y;
      }
      if (keypress_map[SDLK_z].first) {
        new_pos.z = move_to.z + relative_position.z;
      }

      // This is the parent object of the gizmo axis object.
      ptr->translate_identity(*scene, new_pos);

      // This is the actual object you are clicking.
      if (node_link->grab_parent) {
        node_link->parent_get()->translate_identity(*scene, new_pos);
      } else {
        node_link->translate_identity(*scene, new_pos);
      }
    }
  }
}


