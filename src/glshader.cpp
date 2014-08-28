#include "glshader.h"
#include "gldefaults.h"
#include "utils.h"


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
  GL_ASSERT(glAttachShader(program, vs));
  GL_ASSERT(glAttachShader(program, fs));

  GL_ASSERT(glLinkProgram(program));
  GL_ASSERT(glGetProgramiv(program, GL_LINK_STATUS, &link_ok));
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

  GL_ASSERT(glShaderSource(shaderObject, 3, sources, NULL));
  free((void *) source);

  GL_ASSERT(glCompileShader(shaderObject));
  GLint compile_ok = GL_FALSE;
  GL_ASSERT(glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &compile_ok));

  if (compile_ok == GL_FALSE) {
    std::cout << "Fragmic ERROR: could not compile shader: " << filename << std::endl;
    print_log(shaderObject);
    glDeleteShader(shaderObject);
    exit(-1);
    return 0;
  }

  shader_objects.push_back(shaderObject);

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


std::string GLshader::parse_file(const std::string &file)
{
  std::string parsed;
  std::ifstream t(SHADER_PATH + file);

  std::string line;
  while(std::getline(t, line)) {
    std::string add_str;
    std::size_t found = line.find(INCLUDE_STRING);
    if (found != std::string::npos) {
      std::string included_file = line.replace(found, std::string(INCLUDE_STRING).length() + 1, "");

      if (file_exists(SHADER_PATH + included_file) &&
          file.compare(included_file)) {
        add_str = parse_file(included_file);
      } else {
        std::cout << "Error: included GLSL file: '" << included_file << "' is not valid!" << std::endl;
      }
    } else {
      add_str = line + "\r\n";
    }
    parsed.append(add_str);
  }

  t.close();

  return parsed;
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


void GLshader::version_add(std::string &parsed)
{
  parsed.insert(0, std::string(GLSL_VERSION) + "\r\n");
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


std::vector<std::string> GLshader::block_names_get()
{
  GLint numBlocks;
  GLint nameLen;

  std::vector <std::string> name_list;

  glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCKS, &numBlocks);
  name_list.reserve(numBlocks);

  for (int blockIx = 0; blockIx < numBlocks; blockIx++) {
    glGetActiveUniformBlockiv(program, blockIx, GL_UNIFORM_BLOCK_NAME_LENGTH, &nameLen);
    std::vector <GLchar> name;
    name.resize(nameLen);
    glGetActiveUniformBlockName(program, blockIx, nameLen, NULL, &name[0]);
    name_list.push_back(std::string());
    name_list.back().assign(name.begin(), name.end() - 1);       //Remove the null terminator.
  }

  return name_list;
}


void GLshader::print_block_names()
{
  GLint numBlocks;
  GLint nameLen;
  GLint block_binding;
  GLint block_size;
  GLint ref_vertex;
  GLint ref_fragment;

  std::vector <std::string> name_list;

  glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCKS, &numBlocks);
  name_list.reserve(numBlocks);

  std::cout << "Found " << numBlocks << " block(s) in shader" << std::endl;

  for (int blockIx = 0; blockIx < numBlocks; blockIx++) {
    glGetActiveUniformBlockiv(program, blockIx, GL_UNIFORM_BLOCK_NAME_LENGTH, &nameLen);
    std::vector <GLchar> name;
    name.resize(nameLen);
    glGetActiveUniformBlockName(program, blockIx, nameLen, NULL, &name[0]);
    name_list.push_back(std::string());
    name_list.back().assign(name.begin(), name.end() - 1);       //Remove the null terminator.
    glGetActiveUniformBlockiv(program, blockIx, GL_UNIFORM_BLOCK_BINDING, &block_binding);
    glGetActiveUniformBlockiv(program, blockIx, GL_UNIFORM_BLOCK_DATA_SIZE, &block_size);
    glGetActiveUniformBlockiv(program, blockIx, GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER, &ref_vertex);
    glGetActiveUniformBlockiv(program, blockIx, GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER, &ref_fragment);

    std::cout << "Name: " << name.data() << " block binding: " << block_binding << " block size: " << block_size << std::endl;
    std::cout << "ref vertex: " << ref_vertex << ", ref fragment: " << ref_fragment << std::endl;
  }

  for (unsigned int il = 0; il < name_list.size(); il++) {
      std::cout << "Block name: " << name_list[il] << ", index: " <<
      get_block_index(name_list[il]) << std::endl;
  }
}


void GLshader::term()
{
  GL_ASSERT(glDetachShader(program, vs));
  GL_ASSERT(glDetachShader(program, fs));

  for (auto s : shader_objects) {
    GL_ASSERT(glDeleteShader(s));
  }
}


void GLshader::use()
{
  GL_ASSERT(glUseProgram(program));
}
