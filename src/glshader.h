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


  char *fileRead(const char *filename);
  GLuint createShader(std::string fileName, GLenum type);
  void printLog(GLuint object);
  void validate(void);
  void compile(void);


public:
   GLuint program;

   GLshader();
  ~GLshader(void);

  void load(const std::string & vertex, const std::string & fragment);

  void use(void);

  void createDefaultBindings();
  int getBlockIndex(std::string blockName);
  void printBlockNames();
};


#endif
