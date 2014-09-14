#pragma once


#include "camera.h"
#include "console.h"
#include "glcontext.h"
#include "glshader.h"
#include "scene.h"
#include "light.h"


class Ui
{
  private:
    Console *console;
    GLcontext *glcontext;
    Scene *scene;

    Node *node_label_cam = nullptr;
    Node *node_label_fps = nullptr;

    void callback_asset_list(const std::string &prim, const std::string &sec, const std::string &val);
    void callback_object_add(const std::string &prim, const std::string &sec, const std::string &val);
    void callback_node_manipulator(const std::string &prim, const std::string &sec, const std::string &val);
    void callback_scene_list(const std::string &prim, const std::string &sec, const std::string &val);


  public:

    void init(Console &console, GLcontext &glcontext, Scene &scene);
    void draw(const std::string &fps_text);

};
