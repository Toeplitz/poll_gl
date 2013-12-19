#include "indentby.h"

void indentBy(std::ostream &os,size_t indent)
{
  for (size_t i=0; i!=indent; ++i) {
    os << "  ";
  }
}

