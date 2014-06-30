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

  public:
    Scene();
    ~Scene();

    Node                 &load_model(const std::string &prefix, const std::string &filename, bool lefthanded);
    void                  render_list_add(Node &node);
    std::vector<Node *>   render_list_get();
    void                  scene_graph_print();
    void                  scene_graph_print_by_node(Node &node);
    void                  animation_list_add(Node &node);
    std::vector<Node *>   animation_list_get();
    void                  animation_list_update_transforms(Node &node, Uint32 dt);
    void                  upload_queue_add(Node &node);
    Node                 *upload_queue_pop();

    Assets               &assets_get();

};

