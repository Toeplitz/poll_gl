#pragma once

#define UB_GLOBALMATRICES 0
#define UB_MATRICES 1
#define UB_ARMATURE 2
#define UB_MATERIAL 3
#define UB_STATE 4
#define UB_LIGHT 5

#include <GL/glew.h>
#include <GL/glu.h>


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

