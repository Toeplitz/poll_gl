#include "image.h"
#include "stb_image.h"

using std::string;
using std::cout;
using std::endl;

Image::Image()
{
}

Image::~Image()
{
     stbi_image_free(data);
}

bool Image::load(const string &fileName)
{
     data = stbi_load(fileName.c_str(), &width, &height, &components, 3);

     if (data == NULL) {
          cout << "Fragmic error: cannot load image '" << fileName << "'" << endl;
          return false;
     }

     cout << fileName << " loaded, size: " << width << " x " << height << ", " << components << endl;

     return true;
}
