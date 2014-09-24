#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>

#include <cmath>

#include <sys/types.h>
#include <sys/stat.h>
//#include <unistd.h>

#define POLL_DEBUG( os, msg ) \
  (os) << "DEBUG: [" << __FILE__ << ":" << __LINE__ << "] " \
       << msg << std::endl

#define POLL_ERROR( os, msg ) \
  (os) << "ERROR: [" << __FILE__ << ":" << __LINE__ << "] " \
       << msg << std::endl

#define POLL_WARN( os, msg ) \
  (os) << "WARNING: [" << __FILE__ << ":" << __LINE__ << "] " \
       << msg << std::endl


#define NODE_VALIDATE(node) \
  if (!node.active_get()) return;


extern glm::mat4 blender_transform_get();
extern std::string basename(std::string const &pathname);
extern void        indent(std::ostream &os,size_t indent);
bool               file_exists(const std::string &name);
extern void        print_matrix(std::ostream &os,const glm::mat4 &mat,size_t indent);
extern glm::mat4   right_handed_to_left_handed(glm::mat4 &rh);
extern glm::quat   mixQuat(const glm::quat &a, const glm::quat &b, float factor);

