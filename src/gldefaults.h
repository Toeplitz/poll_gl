#pragma once

#include <iostream>
#include <map>

#define UB_GLOBALMATRICES 0
#define UB_MATRICES 1
#define UB_ARMATURE 2
#define UB_STATE 3
#define UB_LIGHT 4
#define UB_MATERIAL 5
#define UB_CONFIG 6

#include <GL/glew.h>
#include <GL/glu.h>


typedef std::map<std::string, int> Uniform_Map;

const Uniform_Map::value_type uniform_buffer_map_def[] = { 
  std::make_pair("GlobalMatrices", UB_GLOBALMATRICES ),
  std::make_pair("Matrices", UB_MATRICES),
  std::make_pair("Armature", UB_ARMATURE), 
  std::make_pair("Node_State", UB_STATE), 
  std::make_pair("Material", UB_MATERIAL), 
  std::make_pair("Light", UB_LIGHT),
  std::make_pair("Config", UB_CONFIG)
};

const Uniform_Map uniform_buffer_map(uniform_buffer_map_def, 
    uniform_buffer_map_def + sizeof uniform_buffer_map_def / sizeof uniform_buffer_map_def[0]);


#ifndef GL_ASSERT
    #ifndef NDEBUG
        #define GL_ASSERT(x)                                    \
            do {                                                \
                GLenum _glError;                                \
                x;                                              \
                _glError = glGetError();                        \
                if(_glError != GL_NO_ERROR) {                   \
                    printf("%s:%d:  %s Error: %s\n",        \
                                __FILE__, __LINE__,             \
                                #x, gluErrorString(_glError)); \
                }                                               \
            } while(__LINE__ == -1)
    #else
        #define GL_ASSERT(x)
    #endif /* NDEBUG */
#endif 


#ifndef CheckGLError
    #define CheckGLError()                              \
        do {                                            \
            GLenum _glError = glGetError();             \
            if(_glError != GL_NO_ERROR) {               \
                printf("%s:%d:  OpenGL Error: %s\n",\
                            __FILE__, __LINE__,         \
                            gluErrorString(_glError));  \
            }                                           \
        } while(__LINE__ == -1)
#endif 

