#ifndef NODE_H
#define NODE_H


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <memory>
#include <string>
#include <vector>

#include "animated.h"
#include "material.h"
#include "mesh.h"


// Forward declaration of Armature
class Armature;

class Node: public Animated {

  private:

  public:
    GLuint gl_vao;

    Armature *armature;
    Material *material;
    Mesh *mesh;

    Node *parent;
    std::string name;
    glm::mat4 globalTransform;
    glm::mat4 currentLocalTransform;
    glm::mat4 originalLocalTransform;
    std::vector <std::unique_ptr <Node>> children;

    bool active;
    int treeLevel;

    Node(const std::string &_name):
      armature(nullptr),
      material(nullptr),
      mesh(nullptr),
      parent(nullptr),
      name(_name),
      currentLocalTransform(1),
      originalLocalTransform(1), 
      active(true), 
      treeLevel(0) {
      } 


    ~Node() 
    {
    }


    void setTreeLevel(int level) {
      treeLevel = level;
    }


    void toggleChild(Node &node) {
      node.active = !node.active;

      for (auto &child : node.children) {
        toggleChild(*child);
      }
    }


    void toggle() {
      active = !active;

      for (auto &node : children) {
        toggleChild(*node);
      }
    }


    void setCurrentLocalTransform(const glm::mat4 &transform) 
    {
      currentLocalTransform = transform;
    }


    void setOriginalLocalTransform(const glm::mat4 &transform) 
    {
      originalLocalTransform = transform;
    }


    void addChild(std::unique_ptr < Node > &&node, int level) 
    {
      node->parent = this;
      node->setTreeLevel(level);
      children.push_back(std::move(node));
    }

};


#endif
