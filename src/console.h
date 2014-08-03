#pragma once

#include "glshader.h"
#include "glcontext.h"
#include "text.h"


class Console
{
  private:

    bool flag_toggle;
    GLshader shader_console;
    GLcontext *glcontext;
    Text text;


  public:

    Console();
    ~Console();

    void init(GLcontext &glcontext);
    void draw();
    void toggle();
};
