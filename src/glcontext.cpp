#include "glcontext.h"


GLcontext::GLcontext()
{
}


GLcontext::~GLcontext()
{
}


bool GLcontext::init(const Window &uiwindow)
{
  glSdlContext = SDL_GL_CreateContext(uiwindow.window);

  const char *error = SDL_GetError();
  if (*error != '\0') {
    std::cout << "GL CONTEXT SDL Error: " << error << std::endl;
    SDL_ClearError();
  }

  if (glewInit() != GLEW_OK) {
    std::cout << "Fragmic ERROR: failed to initalize GLEW" << std::endl;
    return false;
  }

  if (!check_version(3, 0)) {
    std::cout << "Fragmic ERROR: OpenGL version not supported!" << std::endl;
    return false;
  }

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_MULTISAMPLE);
  glEnable(GL_STENCIL_TEST);
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
  glViewport(0, 0, uiwindow.width, uiwindow.height);

  return true;
}


void GLcontext::clear()
{
  glm::vec4 color(1.0, 1.0, 1.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  glClearColor(color.x, color.y, color.z, color.a);
}


void GLcontext::check_error()
{
  try {
    GLenum gl_error = glGetError();
    if (GL_NO_ERROR != gl_error) {
      std::cout << std::string("OpenGL error: ") << reinterpret_cast<const char *>(gluErrorString(gl_error));
      exit(-1);
    }
  } catch(std::exception & e) {
    std::cerr << e.what() << std::endl;
  }

}


bool GLcontext::check_version(const int &major, const int &minor)
{
  int maj, min;

  glGetIntegerv(GL_MAJOR_VERSION, &maj);
  glGetIntegerv(GL_MINOR_VERSION, &min);

  std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
  std::cout << "OpenGL version number: " << maj << "." << min << std::endl;

  if (maj < major)
    return false;

  return true;
}


void GLcontext::polygon_mesh_toggle(bool tog)
{
  glPolygonMode(GL_FRONT_AND_BACK, tog ? GL_FILL : GL_LINE);
}


void GLcontext::swap_interval_set(const int &n)
{
  // 0 for immediate updates, 1 for updates synchronized with
  // the vertical retrace, -1 for late swap tearing
  SDL_GL_SetSwapInterval(n);
}


void GLcontext::swap(const Window &uiwindow)
{
  check_error();
  SDL_GL_SwapWindow(uiwindow.window);
}


void GLcontext::term()
{
  std::cout << "Deleting GL context" << std::endl;
  SDL_GL_DeleteContext(glSdlContext);
}
