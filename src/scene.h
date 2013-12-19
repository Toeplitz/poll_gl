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

#include "buffers.h"
#include "model.h"
#include "node.h"
#include "transform.h"
#include "glrenderer.h"

class Scene {

  private:
    std::vector<Node *> renderQueue;
    std::vector<Node *> transformQueue;
    std::vector<Node *> uploadQueue;

    Assets assets;
    Node root;
    GLrenderer renderer;

    void indentBy(std::ostream & os, size_t indent);

  public:
    Scene();
    ~Scene();

    void init(Buffers &buffers);
    void traverse(Uint32 dt, Node &node);
    void update(Uint32 dt);
    void render();
    Node &loadModel(const std::string &prefix, const std::string &filename);
    void refreshQueue(Node &node);
    void printSceneGraph();
    void printSceneGraphByNode(Node &node);
    void queueTransform(Node & node);
    Node *popUploadQueue();
    void queueUploadNode(Node &node);
    void queueRenderableNode(Node &node);
    std::vector<Node *> getTransformQueue();
    std::vector<Node *> getRenderQueue();
    std::vector<Node *> getUploadQueue();


};

#endif
