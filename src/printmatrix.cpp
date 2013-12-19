#include "printmatrix.h"

#include "indentby.h"

using std::ios;

void printMatrix(std::ostream &os,const glm::mat4 &mat,size_t indent)
{
  ios::fmtflags old_flags = os.setf(ios::fixed);
  std::streamsize old_precision = os.precision(2);
  for (size_t row=0; row!=4; ++row) {
    auto x = mat[row][0];
    auto y = mat[row][1];
    auto z = mat[row][2];
    auto w = mat[row][3];
    indentBy(os,indent);
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
