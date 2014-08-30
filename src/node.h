#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

#include <memory>
#include <string>
#include <vector>

#include "animated.h"
#include "camera.h"
#include "light.h"
#include "physics_rigidbody.h"
#include "manipulator.h"
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


typedef std::vector<std::unique_ptr<Node>> Node_List;


class Node: public Animated {

  private:
    Armature           *armature = nullptr;
    Camera             *camera = nullptr;
    Light              *light = nullptr;
    Physics_Rigidbody  *rigidbody = nullptr;
    Text               *text = nullptr;
    Manipulator        *manipulator = nullptr;
    Material           *material = nullptr;
    Mesh               *mesh = nullptr;
    Node               *parent = nullptr;

    Node_List           children;
    std::string         name;
    Node_State          state;
    int                 tree_level = 0;

    vec3           original_scaling;
    vec3           original_position;
    quat           original_rotation;
    mat4           transform_global = mat4(1.f);
    mat4           transform_local_current = mat4(1.f);
    mat4           transform_local_original = mat4(1.f);
    mat4           transform_model = mat4(1.f);

  public:


    Node(const std::string &node_name);

    Armature           *armature_get();
    void                armature_set(Armature *armature);
    Camera             *camera_create(Assets &assets);
    Camera             *camera_get();
    void                camera_set(Camera *camera);
    void                copy_transform_data(Node &node);
    Node_List    const &children_get() const;
    void                child_add(std::unique_ptr<Node> &&node, int level);
    Light              *light_create(Assets &assets, const unsigned int lamp_type, const unsigned int illumination_type = Light::VOLUME);
    Light              *light_get();
    void                light_set(Light *light);
    const vec3         &original_position_get();
    void                original_position_set(const vec3 &v);
    const quat         &original_rotation_get();
    void                original_rotation_set(const quat &q);
    const vec3         &original_scaling_get();
    void                original_scaling_set(const vec3 &v);
    Node               *parent_get();
    void                print_state(int indent_level);
    Physics_Rigidbody  *physics_rigidbody_create(Assets &assets, Physics &physics);
    Physics_Rigidbody  *physics_rigidbody_get();
    void                physics_rigidbody_set(Physics_Rigidbody *rigidbody);
    const std::string  &name_get();
    void                name_set(const std::string &name);
    Manipulator        *manipulator_create(Assets &assets);
    Manipulator        *manipulator_get();
    void                manipulator_set(Manipulator *manipulator);
    Material           *material_create(Assets &assets);
    Material           *material_get();
    void                material_set(Material *material);
    Mesh               *mesh_create(Assets &assets);
    Mesh               *mesh_get();
    void                mesh_set(Mesh *mesh);
    void                rotate(const float angle, const vec3 &v);
    void                scale(const vec3 &v);
    Node_State         &state_get();
    Text               *text_create(Font *font, Assets &assets);
    Text               *text_get();
    void                text_set(Text *text);
    void                translate(const vec3 &v);
    void                transform_local_current_set(const mat4 &transform);
    void                transform_local_original_set(const mat4 &transform);
    const mat4         &transform_global_get();
    void                transform_global_set(const mat4 &transform);
    const mat4         &transform_local_current_get();
    mat4               &transform_model_get();
    void                transform_model_set(const mat4 &transform);
    void                transform_update_global_recursive(Node &node);
    const int          &tree_level_get();
    void                tree_level_set(const unsigned int &tree_level);
};


