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

    Light *light_active = nullptr;
    Node *node_label_cam = nullptr;
    Node *node_label_fps = nullptr;
    GLshader glshader_ui;


    void callback_light_create(const std::string &prim, const std::string &sec, const std::string &val);
    void callback_light_list(const std::string &prim, const std::string &sec, const std::string &val);

  public:
    Ui();
    ~Ui();

    void init(Console &console, GLcontext &glcontext, Scene &scene);
    void draw(const std::string &fps_text);
    void term();

};
