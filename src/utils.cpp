#include "utils.h"

using std::ios;


void indent(std::ostream &os,size_t indent)
{
  for (size_t i=0; i!=indent; ++i) {
    os << "  ";
  }
}


bool file_exists(const std::string &name) {
  struct stat buffer; 
  return (stat(name.c_str(), &buffer) == 0); 
}


void print_matrix(std::ostream &os, const glm::mat4 &mat, size_t i)
{
  ios::fmtflags old_flags = os.setf(ios::fixed);
  std::streamsize old_precision = os.precision(2);
  for (size_t row=0; row!=4; ++row) {
    auto x = mat[row][0];
    auto y = mat[row][1];
    auto z = mat[row][2];
    auto w = mat[row][3];
    indent(os, i);
    os.width(6);
    os << x << ",";
    os.width(6);
    os << y << ",";
    os.width(6);
    os << z << ",";
    os.width(6);
    os << w << "\n";
  }
  os.flags(old_flags);
  os.precision(old_precision);
}


// Accoriding to:
// http://stackoverflow.com/questions/1263072/changing-a-matrix-from-right-handed-to-left-handed-coordinate-system
glm::mat4 right_handed_to_left_handed(glm::mat4 &rh)
{
  glm::mat4 lh;

  lh[0][0] = rh[0][0];
  lh[1][0] = rh[2][0];
  lh[2][0] = rh[1][0];
  lh[3][0] = rh[3][0];

  lh[0][1] = -rh[0][2];
  lh[1][1] = -rh[2][2];
  lh[2][1] = -rh[1][2];
  lh[3][1] = -rh[3][2];

  lh[0][2] = rh[0][1];
  lh[1][2] = rh[2][1];
  lh[2][2] = rh[1][1];
  lh[3][2] = rh[3][1];

  lh[0][3] = 0.f;
  lh[1][3] = 0.f;
  lh[2][3] = 0.f;
  lh[3][3] = 1.f;

  return lh;
}


glm::quat mixQuat(const glm::quat &a, const glm::quat &b, float factor)
{
  auto dot_product = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
  glm::quat result;
  if (dot_product < 0) {
    // If the dot product is negative, then the two quaternions
    // are pointing in opposite directions, but since 
    // -a is the same rotation as a, we want to negate one of
    // the quaternions to make sure the interpolation is as 
    // clean as possible.
    result.x = glm::mix(-a.x, b.x, factor);
    result.y = glm::mix(-a.y, b.y, factor);
    result.z = glm::mix(-a.z, b.z, factor);
    result.w = glm::mix(-a.w, b.w, factor);
  } else {
    result.x = glm::mix(a.x, b.x, factor);
    result.y = glm::mix(a.y, b.y, factor);
    result.z = glm::mix(a.z, b.z, factor);
    result.w = glm::mix(a.w, b.w, factor);
  }

  // For the quaternion to be a vlid rotation, it needs to be a unit 
  // quaternion.  Our simple interpolation scheme may have denormalized
  // the result.
  result = glm::normalize(result);

  return result;
}
