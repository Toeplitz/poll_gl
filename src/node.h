#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <memory>
#include <string>
#include <vector>

#include "animated.h"
#include "light.h"
#include "material.h"
#include "mesh.h"


class Armature;
class Assets;
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

  public:
    GLuint        gl_vao;
    Armature     *armature;
    Light        *light;
    Material     *material;
    Mesh         *mesh;
    Node         *parent;
    std::string   name;
    glm::mat4     transform_global;
    glm::mat4     transform_local_current;
    glm::mat4     transform_local_original;

    glm::vec3     original_scaling;
    glm::vec3     original_position;
    glm::quat     original_rotation;
    Node_List     children;
    int           tree_level;
    Node_State    state;

    Node(const std::string &_name);
    ~Node();

    void       armature_set(Armature *armature);
    void       copy_transform_data(Node &node);
    void       child_add(std::unique_ptr<Node> &&node, int level);
    Light     *light_create(Assets &assets);
    Light     *light_get();
    void       light_set(Light *light);
    void       local_transform_current_set(const glm::mat4 &transform);
    void       local_transform_original_set(const glm::mat4 &transform);
    void       print_state(int indent_level);
    Mesh      *mesh_create(Assets &assets);
    Mesh      *mesh_get();
    void       mesh_set(Mesh *mesh);
    Material  *material_create(Assets &assets);
    Material  *material_get();
    void       material_set(Material *material);
};
