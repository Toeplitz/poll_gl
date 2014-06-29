#include "glcontext.h"
#include "gldebug.h"



Physics_Debug_Drawer::Physics_Debug_Drawer()
{
  this->debug_mode = 0;
}

Physics_Debug_Drawer::~Physics_Debug_Drawer()
{
}


void Physics_Debug_Drawer::drawLine(const btVector3 &from,const btVector3 &to, const btVector3 &fromColor, const btVector3 &toColor)
{
  GLenum target;
  GLint index;
  std::vector<glm::vec4> vertices;
  std::vector<glm::vec4> colors;

  glm::vec4 start(from.getX(), from.getY(), from.getZ(), 1.0);
  glm::vec4 end(to.getX(), to.getY(), to.getZ(), 1.0);
  vertices.push_back(start);
  vertices.push_back(end);

  glm::vec4 start_color(fromColor.getX(), fromColor.getY(), fromColor.getZ(), 1.0);
  glm::vec4 end_color(toColor.getX(), toColor.getY(), toColor.getZ(), 1.0);
  colors.push_back(start_color);
  colors.push_back(end_color);

  target = GL_ARRAY_BUFFER;

  glGenVertexArrays(1, &gl_vao);
  glBindVertexArray(gl_vao);

  index = 0;
  glGenBuffers(1, &gl_vertex_buffer);
  glBindBuffer(target, gl_vertex_buffer);
  glBufferData(target, vertices.size() * sizeof(vertices[0]), vertices.data(), GL_STATIC_DRAW);
  glEnableVertexAttribArray(index);
  glVertexAttribPointer(index, 4, GL_FLOAT, GL_FALSE, 0, 0);

  index = 1;
  glGenBuffers(1, &gl_color_buffer);
  glBindBuffer(target, gl_color_buffer);
  glBufferData(target, colors.size() * sizeof(colors[0]), colors.data(), GL_STATIC_DRAW);
  glEnableVertexAttribArray(index);
  glVertexAttribPointer(index, 4, GL_FLOAT, GL_FALSE, 0, 0);

  glLineWidth(3);
  glDrawArrays(GL_LINES, 0, 2); 
  glLineWidth(1);
  glBindBuffer(target, 0);
  glDeleteBuffers(1, &gl_vertex_buffer);
  glDeleteBuffers(1, &gl_color_buffer);
  glDeleteVertexArrays(1, &gl_vao);
}


void	Physics_Debug_Drawer::drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color)
{
  drawLine(from,to,color,color);
}


void	Physics_Debug_Drawer::drawSphere(const btVector3 &p, btScalar radius, const btVector3& color)
{
  std::cout << "drawSphere" << std::endl;
}


void	Physics_Debug_Drawer::drawTriangle(const btVector3 &a, const btVector3 &b, const btVector3 &c,const btVector3 &color, btScalar alpha)
{
  std::cout << "drawTriangle" << std::endl;
}


void	Physics_Debug_Drawer::drawContactPoint(const btVector3 &pointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{
  std::cout << "drawContactPoint" << std::endl;
}


void Physics_Debug_Drawer::draw3dText(const btVector3 &location, const char *textString)
{
}


void	Physics_Debug_Drawer::reportErrorWarning(const char *s)
{
  std::cout << s << std::endl;
}


void	Physics_Debug_Drawer::setDebugMode(int debug_mode)
{
  this->debug_mode = debug_mode;
}


int	Physics_Debug_Drawer::getDebugMode() const 
{ 
  return debug_mode;
}

