#include "mixquat.h"

using glm::quat;
using glm::mix;
using glm::normalize;

quat mixQuat(const quat & a, const quat & b, float factor)
{
  auto dot_product = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
  quat result;
  if (dot_product < 0) {
    // If the dot product is negative, then the two quaternions
    // are pointing in opposite directions, but since 
    // -a is the same rotation as a, we want to negate one of
    // the quaternions to make sure the interpolation is as 
    // clean as possible.
    result.x = mix(-a.x, b.x, factor);
    result.y = mix(-a.y, b.y, factor);
    result.z = mix(-a.z, b.z, factor);
    result.w = mix(-a.w, b.w, factor);
  } else {
    result.x = mix(a.x, b.x, factor);
    result.y = mix(a.y, b.y, factor);
    result.z = mix(a.z, b.z, factor);
    result.w = mix(a.w, b.w, factor);
  }

  // For the quaternion to be a vlid rotation, it needs to be a unit 
  // quaternion.  Our simple interpolation scheme may have denormalized
  // the result.
  result = normalize(result);

  return result;
}
