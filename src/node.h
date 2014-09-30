#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using glm::vec3;


#include <memory>
#include <string>
#include <vector>

#include "animated.h"
#include "camera.h"
#include "light.h"
#include "physics_rigidbody.h"
#include "material.h"
#include "mesh.h"
#include "text.h"


class Armature;
class Assets;
class Physics;
class Physics_Rigidbody;
class Node;


struct Node_State 
{
  int animated = false;
  int debug = false;
  int diffuse = false;
  int diffuse_normal = false;
  int diffuse_specular_normal = false;
  int cubemap_reflect = false;
  int cubemap_skybox = false;
  int standard = false;
};


enum Transform_Inherit {
  TRANSFORM_INHERIT_ALL = 0,
  TRANSFORM_INHERIT_POSITION_ONLY = 1
};


typedef std::vector<std::unique_ptr<Node>> Node_List;


class Node: public Animated {

  private:
    Armature           *armature = nullptr;
    Camera             *camera = nullptr;
    Light              *light = nullptr;
    Physics_Rigidbody  *rigidbody = nullptr;
    Text               *text = nullptr;
    Material           *material = nullptr;
    Mesh               *mesh = nullptr;
    Node               *parent = nullptr;

    bool                active = true;
    Node_List           children;
    std::string         name;
    Node_State          state;
    int                 tree_level = 0;

    mat4 transform_position_current = mat4(1.f);

    mat4 transform_global = mat4(1.f);
    mat4 transform_local_current = mat4(1.f);
    mat4 transform_local_original = mat4(1.f);
    mat4 global_transform_rotate = mat4(1.f);
    mat4 global_transform_scale = mat4(1.f);
    mat4 global_transform_translate = mat4(1.f);
    mat4 transform_rotate = mat4(1.f);
    mat4 transform_scale = mat4(1.f);
    mat4 transform_translate = mat4(1.f);

    Transform_Inherit transform_inheritance = TRANSFORM_INHERIT_ALL;
    std::function <void (Node &node, vec3 &position)> raycast_collide_callback = nullptr;

  public:

    mat4 transform_external_local = mat4(1.f);
    mat4 transform_external_global = mat4(1.f);
    

    Node(const std::string &node_name);

    void                active_set(Scene &scene, const bool flag);
    bool                active_get();
    Armature           *armature_get();
    void                armature_set(Armature *armature);
    Camera             *camera_create(Assets &assets);
    Camera             *camera_get();
    void                camera_set(Camera *camera);
    Node_List    const &children_get() const;
    void                child_add(std::unique_ptr<Node> &&node, int level);
    Light              *light_create(Scene &scene, const unsigned int lamp_type, const unsigned int illumination_type = Light::VOLUME);
    Light              *light_get();
    void                light_set(Light *light);
    Node               *parent_get();
    void                print_state(int indent_level);
    Physics_Rigidbody  *physics_rigidbody_create(Scene &scene, bool recursive = true);
    Physics_Rigidbody  *physics_rigidbody_get();
    void                physics_rigidbody_set(Physics_Rigidbody *rigidbody);
    void                physics_rigidbody_update(Scene &scene);
    const std::string  &name_get();
    void                name_set(const std::string &name);
    Material           *material_create(Assets &assets);
    Material           *material_get();
    void                material_set(Material *material);
    Mesh               *mesh_create(Scene &scene);
    Mesh               *mesh_get();
    void                mesh_set(Mesh *mesh);
    mat4               &position_matrix_current_get();
    void                raycast_collide_callback_set(const std::function <void (Node &node, vec3 &position)> raycast_collide_callback);
    void                raycast_collide_callback_call(vec3 &position);
    void                rotate(Scene &scene, const float angle, const vec3 &v);
    void                scale(Scene &scene, const vec3 &v);
    glm::vec3           scale_get();
    glm::vec3           scale_global_get();
    Node_State         &state_get();
    Text               *text_create(Font *font, Scene &scene);
    Text               *text_get();
    void                text_set(Text *text);
    void                translate(Scene &scene, const vec3 &v);
    Transform_Inherit   transform_inheritance_get();
    void                transform_inheritance_set(Transform_Inherit transform_inheritance);
    glm::mat4           transform_full_update(Scene &scene);
    mat4               &transform_global_get();
    mat4               &transform_global_position_get();
    void                transform_global_set(const mat4 &transform);
    mat4               &transform_local_current_get();
    void                transform_local_current_set(Scene &scene, const mat4 &transform);
    void                transform_local_current_set_only(const mat4 &transform);
    mat4               &transform_local_original_get();
    void                transform_local_original_set(const mat4 &transform);
    void                transform_rotate_set(glm::quat &q);
    mat4                transform_rotate_get();
    mat4                transform_scale_get();
    void                transform_scale_set(glm::vec3 &v);
    void                transform_translate_set(glm::vec3 &v);
    mat4                transform_translate_get();
    void                transform_global_rotate_set(const mat4 &transform);
    mat4                transform_global_rotate_get();
    void                transform_global_scale_set(const mat4 &transform);
    mat4                transform_global_scale_get();
    void                transform_global_translate_set(const mat4 &transform);
    mat4                transform_global_translate_get();
    const int          &tree_level_get();
    void                tree_level_set(const unsigned int &tree_level);
};


