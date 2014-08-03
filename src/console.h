#pragma once

#include "glshader.h"
#include "glcontext.h"


class Console
{
  private:

    GLshader shader_console;
    GLcontext *glcontext;

    bool flag_toggle;

  public:

    Console();
    ~Console();

    void init(GLcontext &glcontext);
    void draw();
    void toggle();
};
