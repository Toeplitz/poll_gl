#pragma once

#include <memory>


class Node;
class GLcontext;
class Assets;


class Manipulator_Shape
{
  private:
    std::unique_ptr<Node> center;
    std::unique_ptr<Node> move_gizmo_x;
    std::unique_ptr<Node> move_gizmo_y;
    std::unique_ptr<Node> move_gizmo_z;

};

class Manipulator_Disks: public Manipulator_Shape
{
  public:
    std::unique_ptr<Node> disk_xy;
    std::unique_ptr<Node> disk_xz;
    std::unique_ptr<Node> disk_yz;
};


class Manipulator
{
  private:
    Manipulator_Disks disks;

  public:
    Manipulator();
    ~Manipulator();

    void create_disk(GLcontext &glcontext, Assets &assets);
    void draw();
    void toggle();
};

