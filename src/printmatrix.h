#ifndef PRINTMATRIX_H
#define PRINTMATRIX_H

#include <iostream>
#include <glm/glm.hpp>

extern void printMatrix(std::ostream &os,const glm::mat4 &mat,size_t indent=0);

#endif
