#pragma once

#include <string>
#include <vector>
#include <memory>
#include "assets.h"
#include "manipulator.h"
#include "model.h"
#include "node.h"
#include "physics.h"


class GLcontext;


typedef std::vector<Node *> Node_Ptr_List;


class Scene 
{

  private:
    Node_Ptr_List animated_nodes;
    Node_Ptr_List mesh_nodes;
    Node *node_cur_camera = nullptr;

    Assets assets;
    Physics physics;
    Node root;

    void  animated_nodes_add(Node &node);
    void  mesh_nodes_add(Node &node);
    Node *node_find_recursive(Node &node, const std::string &name);
    void  node_recursive_init(GLcontext &glcontext, Node &node);
    void  node_state_recursive_update(Node &node);

  public:
    Scene();

    const Node_Ptr_List  &animated_nodes_get() const;
    void                  animated_nodes_update_transforms(Node &node, const double dt);
    Assets               &assets_get();
    Camera               *camera_get();
    Node                 &load(GLcontext &glcontext, const std::string &prefix, 
                               const std::string &filename, const unsigned int options);
    const Node_Ptr_List  &mesh_nodes_get() const;
    void                  scene_graph_print(const bool compact = false);
    void                  scene_graph_print_by_node(Node &node, const bool compact = false);
    Node                 *node_camera_get();
    void                  node_camera_set(Node *camera_node);
    Node                 *node_create(const std::string &name, Node *parent = nullptr);
    Node                 *node_find(Node *root_ptr, const std::string &name);
    Node                 &node_root_get();
    Physics              &physics_get();
};

