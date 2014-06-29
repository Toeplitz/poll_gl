#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


extern void        indent(std::ostream &os,size_t indent);
bool               file_exists(const std::string &name);
extern void        print_matrix(std::ostream &os,const glm::mat4 &mat,size_t indent);
extern glm::quat   mixQuat(const glm::quat &a, const glm::quat &b, float factor);

