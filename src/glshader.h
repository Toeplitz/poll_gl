#ifndef GLSHADER_H
#define GLSHADER_H

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
    std::string vertexShaderFile;
    std::string fragmentShaderFile;
    std::string geomShaderFile;

    void    compile();
    GLuint  create_shader(std::string fileName, GLenum type);
    char   *file_read(const char *filename);
    int     get_block_index(std::string blockName);
    void    print_log(GLuint object);
    void    validate();

  public:
    GLuint program;

    GLshader();
    ~GLshader(void);

    void load(const std::string &vertex, const std::string &fragment);
    void print_block_names();
    void use();
};


#endif
