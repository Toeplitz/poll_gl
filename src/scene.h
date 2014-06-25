#pragma once

#include <string>
#include <vector>

#include <SDL2/SDL_stdinc.h>            // for Uint32

#include "model.h"
#include "node.h"

class Scene {

  private:
    std::vector<Node *> render_queue;
    std::vector<Node *> transform_queue;
    std::vector<Node *> upload_queue;

    Assets assets;
    Node root;

  public:
    Scene();
    ~Scene();

    Node                 &load_model(const std::string &prefix, const std::string &filename);
    void                  render_queue_add(Node &node);
    std::vector<Node *>   render_queue_get();
    void                  scene_graph_print();
    void                  scene_graph_print_by_node(Node &node);
    void                  transform_queue_add(Node &node);
    std::vector<Node *>   transform_queue_get();
    void                  transform_update(Node &node, Uint32 dt);
    void                  upload_queue_add(Node &node);
    Node                 *upload_queue_pop();

    Assets               &assets_get();

};

