#include "glcontext.h"
#include "gldebug.h"
#include <glm/gtx/string_cast.hpp>



Physics_Debug_Drawer::Physics_Debug_Drawer()
{
  this->debug_mode = 0;
}

Physics_Debug_Drawer::~Physics_Debug_Drawer()
{
}


void Physics_Debug_Drawer::init()
{
  GLenum target;
  GLint index;
  std::vector<vec3> vertices;
  std::vector<vec3> colors;
  size_t max_size = 900000;

  target = GL_ARRAY_BUFFER;

  glGenVertexArrays(1, &gl_vao);
  glBindVertexArray(gl_vao);

  index = 0;
  glGenBuffers(1, &gl_vertex_buffer);
  glBindBuffer(target, gl_vertex_buffer);
  glBufferData(target, max_size, vertices.data(), GL_STATIC_DRAW);
  glEnableVertexAttribArray(index);
  glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, 0, 0);

  index = 1;
  glGenBuffers(1, &gl_color_buffer);
  glBindBuffer(target, gl_color_buffer);
  glBufferData(target, max_size, colors.data(), GL_STATIC_DRAW);
  glEnableVertexAttribArray(index);
  glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, 0, 0);
}


void Physics_Debug_Drawer::term()
{
  glDeleteBuffers(1, &gl_vertex_buffer);
  glDeleteBuffers(1, &gl_color_buffer);
  glDeleteVertexArrays(1, &gl_vao);
}


void Physics_Debug_Drawer::draw()
{
  GLenum target;

  GL_ASSERT(glBindVertexArray(gl_vao));

  target = GL_ARRAY_BUFFER;
  {
    size_t size = vertices.size() * sizeof(vertices[0]);
    GL_ASSERT(glBindBuffer(target, gl_vertex_buffer));
    GL_ASSERT(glBufferSubData(target, 0, size, vertices.data()));
  }

  {
    size_t size = colors.size() * sizeof(colors[0]);
    GL_ASSERT(glBindBuffer(target, gl_color_buffer));
    GL_ASSERT(glBufferSubData(target, 0, size, colors.data()));
  }


  GL_ASSERT(glEnable(GL_DEPTH_TEST));
  GL_ASSERT(glLineWidth(3));
  GL_ASSERT(glDrawArrays(GL_LINES, 0, vertices.size()););
  GL_ASSERT(glLineWidth(1));
  GL_ASSERT(glDisable(GL_DEPTH_TEST));

  vertices.clear();
  colors.clear();
}


void Physics_Debug_Drawer::drawLine(const btVector3 &from,const btVector3 &to, const btVector3 &fromColor, const btVector3 &toColor)
{
  vec3 start(from.getX(), from.getY(), from.getZ());
  vec3 end(to.getX(), to.getY(), to.getZ());

  vertices.push_back(start);
  vertices.push_back(end);

  vec3 start_color(fromColor.getX(), fromColor.getY(), fromColor.getZ());
  vec3 end_color(toColor.getX(), toColor.getY(), toColor.getZ());
  colors.push_back(start_color);
  colors.push_back(end_color);
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
  btVector3 to = pointOnB + normalOnB * 1;//distance;
  const btVector3 &from = pointOnB;

  drawLine(from, to, color, color);
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

