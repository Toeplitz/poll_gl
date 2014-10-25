#include "plugin_node_tool.h"
#include "physics_rigidbody.h"
#include <glm/gtx/string_cast.hpp>

/*


Usage:


't': translate mode:
's': scale mode
'r': rotate mode

'x': manipulate x-axis sub-mode.
'y': manipulate y-axis sub-mode.
'z': manipulate z-axis sub-mode.


*/


Plugin_Node_Tool::Plugin_Node_Tool(Console &console, Scene &scene)
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

  node_bounding_box = scene.node_create("bounding_box");
  Mesh *mesh = node_bounding_box->mesh_create(scene);
  mesh->generate_line_cube(1.f);
  glcontext.vertex_buffers_mesh_create(mesh);
}


void Plugin_Node_Tool::cb_node_draw(Node &node)
{
  GLcontext &glcontext = scene->glcontext_get();
  Physics_Rigidbody *rigidbody = node.physics_rigidbody_get();

  if (!rigidbody) 
    return;

  mat4 global_translate = node.transform_global_translate_get();

  glDisable(GL_DEPTH_TEST);

  /* DRAW SELECTION OUTLINE */
  Node *node_outline = node_bounding_box;
  Aabb &aabb = node.aabb_get();

  node_outline->transform_global_from_node_set(node, glm::scale(glm::mat4(1.f), aabb.r / node.scale_global_get()));
  glcontext.uniform_buffers_update_matrices(*node_outline);
  glLineWidth(2.f);
  glcontext.draw_mesh(*node_outline);


  /* DRAW GIZMO */
  node_gizmo_translate_x->transform_global_translate_set(global_translate);
  glcontext.uniform_buffers_update_matrices(*node_gizmo_translate_x);
  glcontext.draw_mesh(*node_gizmo_translate_x);

  node_gizmo_translate_y->transform_global_translate_set(global_translate);
  glcontext.uniform_buffers_update_matrices(*node_gizmo_translate_y);
  glcontext.draw_mesh(*node_gizmo_translate_y);

  node_gizmo_translate_z->transform_global_translate_set(global_translate);
  glcontext.uniform_buffers_update_matrices(*node_gizmo_translate_z);
  glcontext.draw_mesh(*node_gizmo_translate_z);

  node_gizmo_translate_x->transform_full_update();
  node_gizmo_translate_y->transform_full_update();
  node_gizmo_translate_z->transform_full_update();

  glEnable(GL_DEPTH_TEST);
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

  if (hitpoint_last_node_get() == node_current) {
    POLL_DEBUG(std::cout, "clicking same node as last, doing nothing");
    return;
  }

  POLL_DEBUG(std::cout, "clicked on new node: " << node_current->name_get());
  node_current->callback_draw_set(std::bind(&Plugin_Node_Tool::cb_node_draw, this, _1));

  if (node_current == node_gizmo_translate_x) {
    keypress_map[SDLK_x].first = true;
    keypress_map[SDLK_t].first = true;
    this->mouse_down = true;
    POLL_DEBUG(std::cout, "This is the X gizmo, do nothing more");
    hitpoints.pop_back();
    hitpoints.push_back(hp);
    return;
  }

  Node *node_last = hitpoint_last_node_get();
  if (node_last) {
    node_last->callback_draw_set(nullptr);
    hitpoints.pop_back();
  }

  node_gizmo_translate_x->link_set(node_current);
  node_gizmo_translate_y->link_set(node_current);
  node_gizmo_translate_z->link_set(node_current);

  hitpoints.push_back(hp);

  /*

     if (hitpoint_last && (hitpoint_last->node_ptr != hp.node_ptr)) {
     Physics_Rigidbody *rb = hitpoint_last->node_ptr->physics_rigidbody_get();
     rb->filter_group_raycast_toggle();
     hitpoint_last->node_ptr->callback_draw_set(nullptr);
     }

     this->hitpoint_last = &hp;
     */
  /*
     hp.node_ptr->callback_draw_set(std::bind(&Plugin_Node_Tool::cb_node_draw, this, _1));
     rb->constraint_create(&hp);
     physics.rigidbody_constraint_add(rb);

*/
}


void Plugin_Node_Tool::cb_mouse_released(SDL_MouseButtonEvent *ev)
{
  Physics &physics = scene->physics_get();
  this->mouse_down = false;
  if (!hitpoint_last)
    return;

  Node *node = hitpoint_last->node_ptr;
  if (!node) {
    POLL_ERROR(std::cerr, "No node attached to hitpoint");
    return;
  }
  Physics_Rigidbody *rb = node->physics_rigidbody_get();
  if (!rb) {
    POLL_ERROR(std::cerr, "No rigidbody on object which was clicked??");
    return;
  }

  physics.rigidbody_constraint_delete(rb);
  rb->constraint_delete();
}


void Plugin_Node_Tool::cb_mouse_motion(SDL_MouseMotionEvent *ev)
{
  if (!mouse_down)
    return;

  Node *node = hitpoint_last_node_get();
  if (!node) {
    POLL_ERROR(std::cerr, "No node attached to the last hipoint.");
    return;
  }

  Node *node_link = node->link_get();
  if (!node_link) {
    POLL_ERROR(std::cerr, "No link node attached to the object.");
    return;
  }

  Physics_Rigidbody *rb = node->physics_rigidbody_get();
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
  float dist = glm::length(hitpoint_last_get()->world_hitpoint - hitpoint_last_get()->ray_from);

  rayFrom = btVector3(camera_pos.x, camera_pos.y, camera_pos.z);
  btVector3 dir = newRayTo - rayFrom;
  dir.normalize();
  dir *= dist;

  btVector3 newPivotB = rayFrom + dir;

  POLL_DEBUG(std::cout, "mouse motion for node: " << node->name_get() << " linked to: " << node_link->name_get());
  if (rb->type_get() == Physics_Rigidbody::KINEMATIC) {
    Node *ptr = node;
    if (node->grab_parent) {
      ptr = node->parent_get();
    }


    if (keypress_map[SDLK_t].first) {

      vec3 new_pos(0, 0, 0);
      mat4 t = ptr->transform_global_translate_get();
      vec3 last_pos = vec3(t[3][0], t[3][1], t[3][2]);
      vec3 diff = vec3(newPivotB.getX(), newPivotB.getY(), newPivotB.getZ()) - last_pos;

      if (keypress_map[SDLK_x].first) {
        new_pos.x = diff.x;
      } 
      if (keypress_map[SDLK_y].first) {
        new_pos.y = diff.y;
      }
      if (keypress_map[SDLK_z].first) {
        new_pos.z = diff.z;
      }

      ptr->translate(*scene, new_pos);
      node_link->translate(*scene, new_pos);
    }
  }
}


void Plugin_Node_Tool::cb_mouse_motion_old(SDL_MouseMotionEvent *ev)
{
  if (!mouse_down)
    return;

  if (!hitpoint_last)
    return;

  Node *node = hitpoint_last->node_ptr;
  if (!node) {
    POLL_ERROR(std::cerr, "No node attached to hitpoint");
    return;
  }
  Physics_Rigidbody *rb = node->physics_rigidbody_get();

  if (!rb) {
    POLL_ERROR(std::cerr, "No rigidbody on object which was clicked??");
    return;
  }

  auto dof6 = rb->bt_dof6_get();

  if (!dof6) {
    POLL_ERROR(std::cerr, "No dof5 object created??");
    return;
  }

  auto height = scene->window_get().height_get();
  auto width = scene->window_get().width_get();
  auto world_ray_anton = raycast.cast_empty(*scene, ev->x, ev->y, width, height);
  auto world_ray = raycast.get_ray_to(*scene, ev->x, ev->y, width, height);

  //POLL_DEBUG(std::cout, "cast_empty: " << glm::to_string(world_ray_anton));
  //POLL_DEBUG(std::cout, "get_ray_to: " << glm::to_string(world_ray));

  btVector3 newRayTo = btVector3(world_ray.x, world_ray.y, world_ray.z);
  //btVector3 newRayTo = btVector3(world_ray_tutor.x, world_ray_tutor.y, world_ray_tutor.z);
  //btVector3 newRayTo = btVector3(world_ray_anton.x, world_ray_anton.y, world_ray_anton.z);
  btVector3 rayFrom;

  vec3 camera_pos = scene->camera_get()->position_get();
  float dist = glm::length(hitpoint_last->world_hitpoint - hitpoint_last->ray_from);

  rayFrom = btVector3(camera_pos.x, camera_pos.y, camera_pos.z);
  btVector3 dir = newRayTo - rayFrom;
  dir.normalize();
  dir *= dist;

  vec3 dir_glm = vec3(dir.getX(), dir.getY(), dir.getZ());
  //POLL_DEBUG(std::cout, "dir: " << glm::to_string(dir_glm));
  btVector3 newPivotB = rayFrom + dir;
  vec3 newPivotB_glm = vec3(newPivotB.getX(), newPivotB.getY(), newPivotB.getZ());
  //POLL_DEBUG(std::cout, "newPivotB: " << glm::to_string(newPivotB_glm));

  if (rb->type_get() == Physics_Rigidbody::KINEMATIC) {
    //node->translate(*scene, glm::vec3(newPivotB.getX(), 0, 0));
    //
    Node *ptr = node;
    if (node->grab_parent) {
      ptr = node->parent_get();
    }


    if (keypress_map[SDLK_t].first) {

      vec3 new_pos(0, 0, 0);
      mat4 t = ptr->transform_global_translate_get();
      vec3 last_pos = vec3(t[3][0], t[3][1], t[3][2]);
      vec3 diff = vec3(newPivotB.getX(), newPivotB.getY(), newPivotB.getZ()) - last_pos;

      if (keypress_map[SDLK_x].first) {
        new_pos.x = diff.x;
      } 
      if (keypress_map[SDLK_y].first) {
        new_pos.y = diff.y;
      }
      if (keypress_map[SDLK_z].first) {
        new_pos.z = diff.z;
      }

      ptr->translate(*scene, new_pos);
    }

    if (keypress_map[SDLK_g].first) {
      vec3 new_pos(1, 1, 1);
      mat4 s;
      if (ptr->grab_parent) { 
        s = ptr->parent_get()->transform_global_scale_get();
      } else {
        s = ptr->transform_global_scale_get();
      }
      vec3 last_scale = vec3(s[0][0], s[1][1], s[2][2]);
      // vec3 diff = glm::abs(vec3(newPivotB.getX(), newPivotB.getY(), newPivotB.getZ()) + last_scale);
      vec3 diff = last_scale + newPivotB.getX();
      //POLL_DEBUG(std::cout, "last_scale: " << glm::to_string(last_scale));
      //POLL_DEBUG(std::cout, "diff: " << glm::to_string(diff));

      if (keypress_map[SDLK_x].first) {
        new_pos.x = diff.x;
      } 
      if (keypress_map[SDLK_y].first) {
        new_pos.y = diff.y;
      }
      if (keypress_map[SDLK_z].first) {
        new_pos.z = diff.z;
      }

      //ptr->scale(*scene, new_pos);
      if (keypress_map[SDLK_LSHIFT].first) {
        ptr->transform_scale_set(new_pos);
      } else {
        new_pos = vec3(new_pos.x, new_pos.x, new_pos.x);
        ptr->transform_scale_set(new_pos);
      }
      scene->transform_update_global_recursive(ptr);
    }

  } else {
    dof6->getFrameOffsetA().setOrigin(newPivotB);
  }

  //POLL_DEBUG(std::cout, "dist: " << dist);

  //printf("newPivotB=%f,%f,%f\n",newPivotB.getX(),newPivotB.getY(),newPivotB.getZ());

}



