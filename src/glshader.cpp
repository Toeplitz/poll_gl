#include "glshader.h"
#include "gldefaults.h"
#include "utils.h"


/**************************************************/
/***************** CONSTRUCTORS *******************/
/**************************************************/


GLshader::GLshader()
{
}


GLshader::~GLshader(void)
{
}


/**************************************************/
/***************** PRIVATE METHODS ****************/
/**************************************************/

void GLshader::compile()
{
  GLint link_ok = GL_FALSE;

  if ((vs = create_shader(vertexShaderFile, GL_VERTEX_SHADER)) == 0)
    return;
  if ((fs = create_shader(fragmentShaderFile, GL_FRAGMENT_SHADER)) == 0)
    return;

  program = glCreateProgram();
  glAttachShader(program, vs);
  glAttachShader(program, fs);

  glLinkProgram(program);
  glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
  if (!link_ok) {
    std::cout << "glLinkProgram: ";
    print_log(program);
    return;
  }
}


GLuint GLshader::create_shader(std::string filename, GLenum type)
{
  const GLchar *source = file_read(filename.c_str());

  if (source == NULL) {
    fprintf(stderr, "Error opening %s: ", filename.c_str());
    perror("");
    return 0;
  }

  GLuint shaderObject = glCreateShader(type);
  const GLchar *sources[] = {
    // Define GLshader version
    ""
      ,
    // GLES2 precision specifiers
#ifdef GL_ES_VERSION_2_0
    // Define default float precision for fragment shaders:
    (type == GL_FRAGMENT_SHADER) ?
      ""
      // Note: OpenGL ES automatically defines this:
      // #define GL_ES
#else
    // Ignore GLES 2 precision specifiers:
    ""
#endif
      ,
    source
  };

  glShaderSource(shaderObject, 3, sources, NULL);
  free((void *) source);

  glCompileShader(shaderObject);
  GLint compile_ok = GL_FALSE;
  glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &compile_ok);

  if (compile_ok == GL_FALSE) {
    std::cout << "Fragmic ERROR: could not compile shader: " << filename << std::endl;
    print_log(shaderObject);
    glDeleteShader(shaderObject);
    exit(-1);
    return 0;
  }

  return shaderObject;
}


char *GLshader::file_read(const char *filename)
{
  FILE *in;

  in = fopen(filename, "rb");

  if (in == NULL)
    return NULL;

  int res_size = BUFSIZ;
  char *res = (char *) malloc(res_size);
  int nb_read_total = 0;

  while (!feof(in) && !ferror(in)) {
    if (nb_read_total + BUFSIZ > res_size) {
      if (res_size > 10 * 1024 * 1024)
        break;
      res_size = res_size * 2;
      res = (char *) realloc(res, res_size);
    }
    char *p_res = res + nb_read_total;
    nb_read_total += fread(p_res, 1, BUFSIZ, in);
  }

  fclose(in);
  res = (char *) realloc(res, nb_read_total + 1);
  res[nb_read_total] = '\0';

  return res;
}


int GLshader::get_block_index(std::string blockName)
{
  GLint ubo_index = glGetUniformBlockIndex(program, blockName.c_str());
  if (ubo_index == -1) {
    std::cout << "Fragmic warning: Could not bind uniform " << ubo_index << " for block '" << blockName << "'" << std::endl;
  }

  return ubo_index;
}


void GLshader::print_log(GLuint object)
{
  GLint log_length = 0;

  if (glIsShader(object))
    glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);
  else if (glIsProgram(object))
    glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);
  else {
    fprintf(stderr, "printlog: Not a shader or a program\n");
    return;
  }

  char *log = (char *) malloc(log_length);

  if (glIsShader(object))
    glGetShaderInfoLog(object, log_length, NULL, log);
  else if (glIsProgram(object))
    glGetProgramInfoLog(object, log_length, NULL, log);

  fprintf(stderr, "%s", log);
  free(log);
}


void GLshader::validate(void)
{
  GLint validate_ok = GL_FALSE;

  glValidateProgram(program);
  glGetProgramiv(program, GL_VALIDATE_STATUS, &validate_ok);
  if (!validate_ok) {
    fprintf(stderr, "glValidateProgram Shader:");
    this->print_log(program);
  }
}


/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/


void GLshader::load(const std::string &vertex, const std::string &fragment, const std::string &geometry)
{
  vertexShaderFile = vertex;
  fragmentShaderFile = fragment;

  if (!file_exists(vertex)) {
    std::cout << "GLSL (vertex shader) file '" << vertex << "' does not exist. Exiting ..." << std::endl;
    exit(-1);
  }
  if (!file_exists(fragment)) {
    std::cout << "GLSL (fragment shader) file '" << fragment << "' does not exist. Exiting ..." << std::endl;
    exit(-1);
  }

  if (!geometry.empty()) {
    if (!file_exists(geometry)) {
      std::cout << "GLSL (geometry shader) file '" << geometry << "' does not exist. Exiting ..." << std::endl;
      exit(-1);
    }
  }

  compile();
  use();
}


void GLshader::print_block_names()
{
  GLint numBlocks;
  GLint nameLen;

  std::vector <std::string> nameList;

  glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCKS, &numBlocks);
  nameList.reserve(numBlocks);

  std::cout << "Found " << numBlocks << " block(s) in shader" << std::endl;

  for (int blockIx = 0; blockIx < numBlocks; blockIx++) {
    glGetActiveUniformBlockiv(program, blockIx, GL_UNIFORM_BLOCK_NAME_LENGTH, &nameLen);
    std::vector <GLchar> name;
    name.resize(nameLen);
    glGetActiveUniformBlockName(program, blockIx, nameLen, NULL, &name[0]);
    nameList.push_back(std::string());
    nameList.back().assign(name.begin(), name.end() - 1);       //Remove the null terminator.
  }

  for (unsigned int il = 0; il < nameList.size(); il++) {
      std::cout << "Block name: " << nameList[il] << ", index: " <<
      get_block_index(nameList[il]) << std::endl;
  }
}


void GLshader::term()
{
  std::cout << "Detatching and deleting GLshader object" << std::endl;

  glDetachShader(program, vs);
  glDetachShader(program, fs);
  glDeleteShader(program);
}


void GLshader::use()
{
  glUseProgram(program);
}
