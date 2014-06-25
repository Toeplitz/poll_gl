#ifndef UTILS_H 
#define UTILS_H

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>


extern void        indent(std::ostream &os,size_t indent);
extern void        printMatrix(std::ostream &os,const glm::mat4 &mat,size_t indent);
extern glm::quat   mixQuat(const glm::quat &a, const glm::quat &b, float factor);


#endif
