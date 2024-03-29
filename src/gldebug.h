#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using glm::vec3;

#include <LinearMath/btIDebugDraw.h>
#include "gldefaults.h"
#include <vector>


class Physics_Debug_Drawer : public btIDebugDraw
{
  GLuint    gl_vertex_buffer;
  GLuint    gl_color_buffer;
  GLuint    gl_vao;
	int       debug_mode;
  std::vector<vec3> vertices;
  std::vector<vec3> colors;

public:

	Physics_Debug_Drawer();
	virtual ~Physics_Debug_Drawer(); 
  void init();
  void term();

	virtual void	drawLine(const btVector3 &from,const btVector3 &to, const btVector3 &fromColor, const btVector3 &toColor);
	virtual void	drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color);
	virtual void	drawSphere(const btVector3 &p, btScalar radius, const btVector3& color);
	virtual void	drawTriangle(const btVector3 &a, const btVector3 &b, const btVector3 &c,const btVector3 &color, btScalar alpha);
	virtual void	drawContactPoint(const btVector3 &pointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime, const btVector3& color);
	virtual void	reportErrorWarning(const char *warningString);
	virtual void	draw3dText(const btVector3 &location, const char *textString);
  virtual void	setDebugMode(int debug_mode);
  virtual int	  getDebugMode() const;
          void  draw();

};

