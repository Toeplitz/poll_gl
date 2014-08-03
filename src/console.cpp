#include "console.h"


/**************************************************/
/***************** CONSTRUCTORS *******************/
/**************************************************/


Console::Console():
  flag_toggle(false)
{
}


Console::~Console()
{
}


/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/


void Console::init(GLcontext &glcontext)
{
  this->glcontext = &glcontext;
}


void Console::draw()
{
  if (!flag_toggle)
    return;


  std::cout << "displaying console" << std::endl;

}


void Console::toggle()
{
  flag_toggle = !flag_toggle;
}


