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

class GLshader {

  private:
    GLuint vs;
    GLuint fs;
    GLuint gs;

    void    compile();
    GLuint  create_shader(std::string fileName, GLenum type);
    char   *file_read(const char *filename);
    void    print_log(GLuint object);
    void    validate();

  public:
    std::string vertexShaderFile;
    std::string fragmentShaderFile;
    std::string geomShaderFile;
    GLuint program;

    GLshader();
    ~GLshader(void);

    std::vector<std::string> block_names_get();
    void load(const std::string &vertex, const std::string &fragment, const std::string &geometry = "");
    int  get_block_index(std::string blockName);
    void print_block_names();
    void term();
    void use();
};


