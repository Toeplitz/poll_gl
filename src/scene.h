#pragma once

#include <string>
#include <vector>
#include <memory>
#include "assets.h"
#include "model.h"
#include "node.h"
#include "poll_plugin.h"
#include "physics.h"


class GLcontext;
class Window;


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

    Poll_Plugin_List *plugins_ptr;
    Window *window_ptr = nullptr;

    void  animated_nodes_add(Node &node);
    Node *node_find_recursive(Node &node, const std::string &name);
    void  node_recursive_init(GLcontext &glcontext, Node &node);
    void  node_state_recursive_update(Node &node);

  public:
    Scene();

    const Node_Ptr_List    &animated_nodes_get() const;
    void                    animated_nodes_update_transforms(Node &node, const double dt);
    Assets                 &assets_get();
    Camera                 *camera_get();
    GLcontext              &glcontext_get();
    void                    init(Poll_Plugin_List &plugins, Window &window);
    Node                   &load(const std::string &prefix,  const std::string &filename, const unsigned int options);
    void                    mesh_nodes_add(Node &node);
    const Node_Ptr_List    &mesh_nodes_get() const;
    void                    scene_graph_print(const bool compact = false);
    void                    scene_graph_print_by_node(Node &node, const bool compact = false);
    Node                   *node_camera_get();
    void                    node_camera_set(Node *camera_node);
    Node                   *node_create(const std::string &name, Node *parent = nullptr, Transform_Inherit transform_inheritance = TRANSFORM_INHERIT_ALL);
    Node                   *node_find(Node *root_ptr, const std::string &name);
    void                    node_positions_update_recursive(Node &node);
    void                    node_reset_transforms_recursive(Node &node);
    Node                   &node_root_get();
    Physics                &physics_get();
    const Poll_Plugin_List &plugins_get() const;
    void                    transform_update_global_recursive(Node *node);
    Window                 &window_get();
};

