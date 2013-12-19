#include "scene.h"


void Scene::indentBy(std::ostream & os, size_t indent) 
{
  for (size_t i = 0; i != indent; ++i) {
    os << "  ";
  }
} 


Scene::Scene(): 
  assets(), 
  root(std::string("Fragmic")) ,
  renderer()
{
}


Scene::~Scene() 
{
}

void Scene::init(Buffers &buffers)
{
  renderer.init(buffers);
}


void Scene::traverse(Uint32 dt, Node &node)
{
  glm::mat4 transform = node.currentLocalTransform;
  Node *parent = node.parent;

  if (node.getNumKeyFrames() != 0) {
    node.stepTime((double) dt / 1000.0);
    node.setCurrentLocalTransform(node.localInterpolatedMatrix);
  }

  if (parent) {
    node.globalTransform = parent->globalTransform * transform;
  } else {
    node.globalTransform = transform;
  }

  for (auto &child : node.children) {
    traverse(dt, *child);
  }
}


void Scene::update(Uint32 dt)
{
  std::lock_guard<std::mutex> lock(assets.getMutex());

  Node *ptr = popUploadQueue();
  while (ptr) {
    renderer.upload(*ptr);
    ptr = popUploadQueue();
  }

  for (auto &node : getTransformQueue()) {
    traverse(dt, *node);
  }

  for (auto &armature : assets.getArmatures()) {
    armature->updateBones();
  }
}


void Scene::render()
{
  for (auto &node : getRenderQueue()) {
    renderer.draw(*node);
  }
}


Node &Scene::loadModel(const std::string &prefix, const std::string &filename) 
{
  Transform transform;

  Model model;
  Node *rootPtr = model.load(assets, root, prefix, filename);

  transform.calculateGlobalTransformTopDown(root);
  refreshQueue(*rootPtr);

  queueUploadNode(*rootPtr);

  return *rootPtr;
}


void Scene::refreshQueue(Node &node) 
{
  if (node.mesh) {
    queueRenderableNode(node);
  } else if (node.armature) {
    queueTransform(node);
  }

  for (auto & child:node.children) {
    refreshQueue(*child);
  }
}


void Scene::printSceneGraph() 
{
  printSceneGraphByNode(root);
}


void Scene::printSceneGraphByNode(Node &node)
{
  if (node.active) {
    indentBy(std::cout, node.treeLevel);
    std::cout << node.treeLevel << ": '" << node.
      name << "'" << &node << "";

    if (node.mesh) {
      std::cout << " (mesh)";
    }

    if (node.armature) {
      std::cout << " (armature)";
    }
    std::cout << std::endl;

  }

  for (auto &child : node.children) {
    printSceneGraphByNode(*child);
  }
}


void Scene::queueTransform(Node &node) 
{
  bool isPresent = (std::find(transformQueue.begin(), 
                              transformQueue.end(), 
                              &node) != transformQueue.end());
  if (isPresent) {
    std::cout << "Node is already in transform queue." << std::endl;
    return;
  }

  transformQueue.push_back(&node);
}


Node *Scene::popUploadQueue()
{
  if (uploadQueue.empty()) {
    return nullptr;
  }

  Node *node = uploadQueue.back();
  if (!node) {
    std::cout << "No item left in vector" << std::endl;
  }

  uploadQueue.pop_back();

  return node;
}


void Scene::queueUploadNode(Node &node) 
{
  if (node.mesh) {
    uploadQueue.push_back(&node);
  }

  for (auto &child : node.children) {
    queueUploadNode(*child);
  }
}


void Scene::queueRenderableNode(Node &node) 
{
  renderQueue.push_back(&node);
}


std::vector <Node *> Scene::getTransformQueue() 
{
  return transformQueue;
}


std::vector <Node *> Scene::getRenderQueue() 
{
  return renderQueue;
}


std::vector <Node *> Scene::getUploadQueue() 
{
  return uploadQueue;
}
