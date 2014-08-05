#pragma once

#include <string>
#include <vector>

#include <SDL2/SDL_stdinc.h>            // for Uint32

#include "model.h"
#include "node.h"

typedef std::vector<Node *> Node_Ptr_List;

class Scene {

  private:
    Node_Ptr_List animated_nodes;
    Node_Ptr_List mesh_nodes;
    Node_Ptr_List upload_queue;

    Node *node_cur_camera;

    Assets assets;
    Node root;

    Node *node_find_recursive(Node &node, const std::string &name);

  public:
    Scene();
    ~Scene();

    Assets                     &assets_get();
    void                        animated_nodes_add(Node &node);
    const std::vector<Node *>  &animated_nodes_get() const;
    void                        animated_nodes_update_transforms(Node &node, const double dt);
    Camera                     *camera_get();
    void                        mesh_nodes_add(Node &node);
    const std::vector<Node *>  &mesh_nodes_get() const;
    Node                       &model_load(const std::string &prefix, const std::string &filename, 
                                           const unsigned int options = MODEL_IMPORT_DEFAULT);
    Node                       *node_find(Node *root_ptr, const std::string &name);
    void                        scene_graph_print(const bool compact = false);
    void                        scene_graph_print_by_node(Node &node, const bool compact = false);
    Node                       *node_camera_get();
    void                        node_camera_set(Node *camera_node);
    Node                       *node_create(const std::string &name, Node *parent = nullptr);
    Node                       &node_root_get();
    void                        state_update_recursive(Node &node);
    void                        upload_queue_add(Node &node);
    Node                       *upload_queue_pop();
};

