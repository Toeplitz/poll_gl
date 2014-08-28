#pragma once

#include <iostream>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <memory>
#include <cstdlib>
#include <vector>

#include "gldefaults.h"


#define INCLUDE_STRING "#include"
#define SHADER_PATH "/home/ms/git/poll/shaders/"
#define GLSL_VERSION "#version 330"


class GLshader {

  private:
    GLuint vs;
    GLuint fs;
    GLuint gs;
    std::vector<GLuint> shader_objects;

    void        compile();
    GLuint      create_shader_cpp(const std::string &file, GLenum type);
    GLuint      create_shader(std::string fileName, GLenum type);
    char       *file_read(const char *filename);
    std::string parse_file(const std::string &file);
    void        print_log(GLuint object);
    void        validate();
    void        version_add(std::string &parsed);

  public:
    std::string vertexShaderFile;
    std::string fragmentShaderFile;
    std::string geomShaderFile;
    GLuint program;

    std::vector<std::string> block_names_get();
    void                     load(const std::string &vertex, const std::string &fragment, const std::string &geometry = "");
    int                      get_block_index(std::string blockName);
    void                     print_block_names();
    void                     term();
    void                     use();
};


