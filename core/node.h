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
    Node               *link = nullptr;

    bool                active = true;
    Node_List           children;
    std::string         name;
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
    std::function <void (Node &node, vec3 &position)> callback_raycast_collide = nullptr;
    std::function <void (Node &node)> callback_draw = nullptr;
    std::shared_ptr<Aabb> aabb;
    short raycast_priority = 0;
    bool  shadow_cast = true;

  public:

    unsigned int import_options = 0;
    bool         grab_parent = false;
    

    Node(const std::string &node_name);


    Aabb               &aabb_get();
    void                active_set(Scene &scene, const bool flag);
    bool                active_get();
    Armature           *armature_get();
    void                armature_set(Armature *armature);
    void                callback_draw_call();
    void                callback_draw_set(const std::function <void (Node &node)> callback_draw);
    void                callback_raycast_collide_call(vec3 &position);
    void                callback_raycast_collide_set(const std::function <void (Node &node, vec3 &position)> callback_raycast_collide);
    Camera             *camera_create(Assets &assets);
    Camera             *camera_get();
    void                camera_set(Camera *camera);
    Node_List    const &children_get() const;
    void                child_add(std::unique_ptr<Node> &&node, int level);
    Light              *light_create(Scene &scene, const unsigned int lamp_type, const unsigned int illumination_type = Light::VOLUME);
    Light              *light_get();
    void                light_set(Light *light);
    void                link_set(Node *node);
    Node               *link_get();
    Node               *parent_get();
    vec3                position_get();
    vec3                position_local_get();
    Physics_Rigidbody  *physics_rigidbody_create(Scene &scene, bool recursive = true);
    Physics_Rigidbody  *physics_rigidbody_get();
    void                physics_rigidbody_set(Physics_Rigidbody *rigidbody);
    void                physics_rigidbody_update();
    const std::string  &name_get();
    void                name_set(const std::string &name);
    Material           *material_create(Assets &assets);
    Material           *material_get();
    void                material_set(Material *material);
    Mesh               *mesh_create(Scene &scene);
    Mesh               *mesh_get();
    void                mesh_set(Mesh *mesh);
    short               raycast_priority_get();
    void                raycast_priority_set(const short priority);
    void                rotate(Scene &scene, const float angle, const vec3 &v);
    void                scale(Scene &scene, const vec3 &v);
    void                scale_identity(Scene &scene, const vec3 &v);
    glm::vec3           scale_get();
    glm::vec3           scale_global_get();
    bool                shadow_cast_get();
    void                shadow_cast_set(Scene &scene, const bool shadow_cast);
    Text               *text_create(Font *font, Scene &scene);
    Text               *text_get();
    void                text_set(Text *text);
    void                translate(Scene &scene, const vec3 &v);
    void                translate_identity(Scene &scene, const vec3 &v);
    Transform_Inherit   transform_inheritance_get();
    void                transform_inheritance_set(Transform_Inherit transform_inheritance);
    glm::mat4           transform_full_update();
    mat4               &transform_local_current_get();
    void                transform_local_current_set(const mat4 &transform);
    void                transform_local_current_set_only(const mat4 &transform);
    mat4               &transform_local_original_get();
    void                transform_local_original_set(const mat4 &transform);
    void                transform_rotate_set(glm::quat &q);
    mat4                transform_rotate_get();
    mat4                transform_scale_get();
    void                transform_scale_set(glm::vec3 &v);
    void                transform_translate_set(glm::vec3 &v);
    mat4                transform_translate_get();
    mat4               &transform_global_get();
    void                transform_global_set(const mat4 &transform);
    void                transform_global_from_node_set(Node &node, const mat4 &transform);
    void                transform_global_rotate_set(const mat4 &transform);
    mat4                transform_global_rotate_get();
    void                transform_global_scale_set(const mat4 &transform);
    mat4                transform_global_scale_get();
    void                transform_global_translate_set(const mat4 &transform);
    mat4                transform_global_translate_get();
    const int          &tree_level_get();
    void                tree_level_set(const unsigned int &tree_level);
};


