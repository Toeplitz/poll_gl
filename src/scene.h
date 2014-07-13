#pragma once

#include <string>
#include <vector>

#include <SDL2/SDL_stdinc.h>            // for Uint32

#include "model.h"
#include "node.h"

typedef std::vector<Node *> Node_Ptr_List;

class Scene {

  private:
    Node_Ptr_List render_list;
    Node_Ptr_List animation_list;
    Node_Ptr_List upload_queue;

    Assets assets;
    Node root;

    Node *node_find_recursive(Node &node, const std::string &name);

  public:
    Scene();
    ~Scene();

    void                  animation_list_add(Node &node);
    std::vector<Node *>   animation_list_get();
    void                  animation_list_update_transforms(Node &node, const double dt);
    Assets               &assets_get();
    Node                 &model_load(const std::string &prefix, const std::string &filename, bool draw = true);
    Node                 *node_find(Node *root_ptr, const std::string &name);
    void                  render_list_add(Node &node);
    std::vector<Node *>   render_list_get();
    void                  scene_graph_print();
    void                  scene_graph_print_by_node(Node &node);
    Node                 *node_create(const std::string &name);
    Node                 &node_root_get();
    void                  state_update_recursive(Node &node);
    void                  upload_queue_add(Node &node);
    Node                 *upload_queue_pop();
};

