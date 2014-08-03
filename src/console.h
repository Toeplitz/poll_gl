#pragma once

#include "glshader.h"
#include "glcontext.h"
#include "scene.h"
#include "text.h"


class Console
{
  private:

    bool flag_toggle;
    GLshader glshader_console;
    GLcontext *glcontext;
    Text text;
    Node *node;


  public:

    Console();
    ~Console();

    void init(Scene &scene, GLcontext &glcontext);
    void draw();
    void toggle();
};
