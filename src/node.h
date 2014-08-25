#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
class Physics_Rigidbody;
class Node;


struct Node_State 
{
  int animated;
  int debug;
  int diffuse;
  int diffuse_normal;
  int diffuse_specular_normal;
  int cubemap_reflect;
  int cubemap_skybox;
  int standard;
};


typedef std::vector<std::unique_ptr<Node>> Node_List;


class Node: public Animated {

  private:
    Camera             *camera = nullptr;
    Light              *light = nullptr;
    Physics_Rigidbody  *rigidbody = nullptr;
    Text               *text = nullptr;
    Manipulator        *manipulator = nullptr;

  public:
    Armature           *armature = nullptr;
    Material           *material = nullptr;
    Mesh               *mesh = nullptr;
    Node               *parent = nullptr;

    std::string         name;
    glm::mat4           transform_global;
    glm::mat4           transform_local_current;
    glm::mat4           transform_local_original;
    glm::mat4           transform_model = glm::mat4(1.f);

    glm::vec3           original_scaling;
    glm::vec3           original_position;
    glm::quat           original_rotation;
    Node_List           children;
    int                 tree_level = 0;
    Node_State          state;

    Node(const std::string &node_name);
    ~Node();

    Armature           *armature_get();
    void                armature_set(Armature *armature);
    Camera             *camera_create(Assets &assets);
    Camera             *camera_get();
    void                camera_set(Camera *camera);
    void                copy_transform_data(Node &node);
    void                child_add(std::unique_ptr<Node> &&node, int level);
    Light              *light_create(Assets &assets);
    Light              *light_get();
    void                light_set(Light *light);
    void                print_state(int indent_level);
    Physics_Rigidbody  *physics_rigidbody_create(Assets &assets);
    Physics_Rigidbody  *physics_rigidbody_get();
    void                physics_rigidbody_set(Physics_Rigidbody *rigidbody);
    Manipulator        *manipulator_create(Assets &assets);
    Manipulator        *manipulator_get();
    void                manipulator_set(Manipulator *manipulator);
    Material           *material_create(Assets &assets);
    Material           *material_get();
    void                material_set(Material *material);
    Mesh               *mesh_create(Assets &assets);
    Mesh               *mesh_get();
    void                mesh_set(Mesh *mesh);
    void                rotate(const float angle, const glm::vec3 &v);
    void                scale(const glm::vec3 &v);
    Text               *text_create(Font *font, Assets &assets);
    Text               *text_get();
    void                text_set(Text *text);
    void                translate(const glm::vec3 &v);
    void                transform_local_current_set(const glm::mat4 &transform);
    void                transform_local_original_set(const glm::mat4 &transform);
    glm::mat4          &transform_model_get();
    void                transform_model_set(const glm::mat4 &transform);
    void                transform_update_global_recursive(Node &node);
};
