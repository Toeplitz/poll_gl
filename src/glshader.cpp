#include "glshader.h"
#include "gldefaults.h"

using std::string;
using std::cout;
using std::endl;
using std::vector;



char *GLshader::fileRead(const char *filename)
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

GLuint GLshader::createShader(string fileName, GLenum type)
{
  const GLchar *source = fileRead(fileName.c_str());

  if (source == NULL) {
    fprintf(stderr, "Error opening %s: ", fileName.c_str());
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
    std::cout << "Fragmic ERROR: could not compile shader!" << std::endl;
    printLog(shaderObject);
    glDeleteShader(shaderObject);
    exit(-1);
    return 0;
  }

  return shaderObject;
}

void GLshader::printLog(GLuint object)
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
    this->printLog(program);
  }
}


GLshader::GLshader()
{
  cout << "Constructing GLshader object" << endl;
}

GLshader::~GLshader(void)
{
  cout << "Detatching and deleting GLshader object" << endl;

  glDetachShader(program, vs);
  glDetachShader(program, fs);
  glDeleteShader(program);
}

void GLshader::load(const string & vertex, const string & fragment)
{
  vertexShaderFile = vertex;
  fragmentShaderFile = fragment;

  compile();
}

void GLshader::compile()
{
  GLint link_ok = GL_FALSE;

  if ((vs = this->createShader(vertexShaderFile, GL_VERTEX_SHADER)) == 0)
    return;
  if ((fs = this->createShader(fragmentShaderFile, GL_FRAGMENT_SHADER)) == 0)
    return;

  program = glCreateProgram();
  glAttachShader(program, vs);
  glAttachShader(program, fs);

  glLinkProgram(program);
  glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
  if (!link_ok) {
    cout << "glLinkProgram: ";
    printLog(program);
    return;
  }
}

void GLshader::use()
{
  glUseProgram(program);
}

int GLshader::getBlockIndex(std::string blockName)
{
  GLint uboIndex = glGetUniformBlockIndex(program, blockName.c_str());
  if (uboIndex == -1) {
    std::cout << "Fragmic warning: Could not bind uniform " << uboIndex << std::
      endl;
  }

  return uboIndex;
}

void GLshader::createDefaultBindings()
{
  GLint uniform_block_index;

  uniform_block_index = glGetUniformBlockIndex(program, "GlobalMatrices");
  glUniformBlockBinding(program, uniform_block_index, UB_GLOBALMATRICES);

  uniform_block_index = glGetUniformBlockIndex(program, "Matrices");
  glUniformBlockBinding(program, uniform_block_index, UB_MATRICES);

  uniform_block_index = glGetUniformBlockIndex(program, "Armature");
  glUniformBlockBinding(program, uniform_block_index, UB_ARMATURE);

  uniform_block_index = glGetUniformBlockIndex(program, "Material");
  glUniformBlockBinding(program, uniform_block_index, UB_MATERIAL);

  uniform_block_index = glGetUniformBlockIndex(program, "Light");
  glUniformBlockBinding(program, uniform_block_index, UB_LIGHT);

  uniform_block_index = glGetUniformBlockIndex(program, "Debug");
  glUniformBlockBinding(program, uniform_block_index, UB_DEBUG);
}

void GLshader::printBlockNames()
{

  GLint numBlocks;
  GLint nameLen;

  std::vector < std::string > nameList;

  glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCKS, &numBlocks);
  nameList.reserve(numBlocks);


  std::cout << "Found " << numBlocks << " block(s) in shader" << std::endl;

  for (int blockIx = 0; blockIx < numBlocks; blockIx++) {
    glGetActiveUniformBlockiv(program, blockIx, GL_UNIFORM_BLOCK_NAME_LENGTH,
                              &nameLen);

    std::vector < GLchar > name;
    name.resize(nameLen);
    glGetActiveUniformBlockName(program, blockIx, nameLen, NULL, &name[0]);

    nameList.push_back(std::string());
    nameList.back().assign(name.begin(), name.end() - 1);       //Remove the null terminator.

  }
  for (unsigned int il = 0; il < nameList.size(); il++) {
    std::
      cout << "Block name: " << nameList[il] << ", index: " <<
      getBlockIndex(nameList[il]) << std::endl;
  }

}
