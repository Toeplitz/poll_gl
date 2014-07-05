#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <memory>
#include <string>
#include <vector>

#include "animated.h"
#include "material.h"
#include "mesh.h"


class Armature;
class Node;


struct Node_State 
{
  int animated;
  int debug;
  int diffuse;
  int diffuse_normal;
  int diffuse_specular_normal;
  int standard;
};


typedef std::vector<std::unique_ptr<Node>> Node_List;


class Node: public Animated {
  private:

  public:
    GLuint        gl_vao;
    Armature     *armature;
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

    void copy_transform_data(Node &node);
    void child_add(std::unique_ptr<Node> &&node, int level);
    void print_state(int indent_level);
    void local_transform_current_set(const glm::mat4 &transform);
    void local_transform_original_set(const glm::mat4 &transform);
};
