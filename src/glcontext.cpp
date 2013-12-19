#include "glcontext.h"

using std::cout;
using std::endl;
using std::string;
using glm::vec4;
using glm::vec3;

GLcontext::GLcontext()
{
}

GLcontext::~GLcontext()
{
}

bool GLcontext::init(const UIwindow & uiwindow)
{
  glSdlContext = SDL_GL_CreateContext(uiwindow.window);

  const char *error = SDL_GetError();
  if (*error != '\0') {
    cout << "SDL Error: " << error << endl;
    SDL_ClearError();
    return false;
  }

  if (glewInit() != GLEW_OK) {
    cout << "Fragmic ERROR: failed to initalize GLEW" << endl;
    return false;
  }

  if (!checkVersion(3, 3)) {
    cout << "Fragmic ERROR: OpenGL version not supported!" << endl;
    exit(-1);
  }

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_MULTISAMPLE);
  glEnable(GL_STENCIL_TEST);
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
  glViewport(0, 0, uiwindow.width, uiwindow.height);

//  glEnable (GL_CULL_FACE); // cull face
//  glCullFace (GL_BACK); // cull back face
//  glFrontFace (GL_CW); // GL_CCW for counter clock-wise

  return true;
}

void GLcontext::clear()
{
  glm::vec4 color(1.0, 1.0, 1.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  glClearColor(color.x, color.y, color.z, color.a);
}

void GLcontext::checkError()
{
  try {
    GLenum gl_error = glGetError();
    if (GL_NO_ERROR != gl_error) {

      throw std::runtime_error(std::string("OpenGL error: ") +
                               reinterpret_cast <
                               const char *>(gluErrorString(gl_error)));

    }
  } catch(std::exception & e) {
    std::cerr << e.what() << std::endl;
  }

}

bool GLcontext::checkVersion(const int &major, const int &minor)
{
  int maj, min;

  glGetIntegerv(GL_MAJOR_VERSION, &maj);
  glGetIntegerv(GL_MINOR_VERSION, &min);

  cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;
  cout << "OpenGL version number: " << maj << "." << min << endl;

  if (maj < major)
    return false;

  return true;
}

void GLcontext::togglePolygonMesh(bool tog)
{
  glPolygonMode(GL_FRONT_AND_BACK, tog ? GL_FILL : GL_LINE);
}

void GLcontext::setSwapInterval(const int &n)
{
  // 0 for immediate updates, 1 for updates synchronized with
  // the vertical retrace, -1 for late swap tearing
  SDL_GL_SetSwapInterval(n);
}

void GLcontext::swap(const UIwindow & uiwindow)
{
  SDL_GL_SwapWindow(uiwindow.window);
}

void GLcontext::term()
{
  SDL_GL_DeleteContext(glSdlContext);
}
