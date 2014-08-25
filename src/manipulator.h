#pragma once

#include <memory>


class GLcontext;
class Assets;


class Manipulator
{
  private:

  public:
    Manipulator();
    ~Manipulator();

    void create_disk(GLcontext &glcontext, Assets &assets);
    void draw();
    void toggle();
};

