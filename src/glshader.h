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
    std::string vertex_shader_file;
    std::string fragment_shader_file;
    std::string geometry_shader_file;
    GLuint program;

    void        compile();
    GLuint      create_shader(std::string fileName, GLenum type);
    std::string parse_file(const std::string &file);
    void        print_log(GLuint object);
    void        print_source(std::string &source);
    void        validate();
    void        version_add(std::string &parsed);

  public:

    std::vector<std::string> block_names_get();
    void                     load(const std::string &vertex, const std::string &fragment, const std::string &geometry = "");
    int                      get_block_index(std::string blockName);
    void                     print_block_names();
    void                     print_subroutines();
    GLuint                  &program_get();
    void                     term();
    void                     use();
};


