#include "keyframe.h"


/**************************************************/
/***************** CONSTRUCTORS *******************/
/**************************************************/

KeyFrame::KeyFrame(glm::vec3 _s, glm::quat _q, glm::vec3 _t, double _time)
  : time(_time),
  s(_s),
  q(_q), 
  t(_t) 
{
}


KeyFrame::~KeyFrame() 
{

}
