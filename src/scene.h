#ifndef SCENE_H
#define SCENE_H

#include <memory>
#include <string>
#include <vector>
#include <deque>
#include <thread>
#include <mutex>
#include <future>
#include <thread>
#include <chrono>
#include <iostream>


#include <SDL2/SDL.h>

#include "model.h"
#include "node.h"
#include "transform.h"

class Scene {

  private:
    std::vector<Node *> render_queue;
    std::vector<Node *> transform_queue;
    std::vector<Node *> upload_queue;

    Assets assets;
    Node root;

    void                  indent(std::ostream &os, size_t indent);

  public:
    Scene();
    ~Scene();

    Assets               &assets_get();
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



};

#endif
