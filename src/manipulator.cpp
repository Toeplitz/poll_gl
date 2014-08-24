#include "manipulator.h"
#include "node.h"


/**************************************************/
/***************** CONSTRUCTORS *******************/
/**************************************************/


Manipulator::Manipulator()
{
}


Manipulator::~Manipulator()
{
}


/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/


void Manipulator::create_disk(GLcontext &glcontext, Assets &assets)
{
  std::unique_ptr<Node> disk_xy_ptr(new Node("disk_xy"));
  std::unique_ptr<Node> disk_xz_ptr(new Node("disk_xz"));
  std::unique_ptr<Node> disk_yz_ptr(new Node("disk_yz"));

  disk_xy_ptr->mesh_create(assets);

  disks.disk_xy = std::move(disk_xy_ptr);
  disks.disk_xz = std::move(disk_xz_ptr);
  disks.disk_yz = std::move(disk_yz_ptr);

}


void Manipulator::draw()
{

}


void Manipulator::toggle()
{
}

