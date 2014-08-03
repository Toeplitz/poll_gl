#include "text.h"
#include <stdio.h>

#define STB_TRUETYPE_IMPLEMENTATION 
#include "../external/stb_truetype.h"

static stbtt_bakedchar cdata[96];


/**************************************************/
/***************** CONSTRUCTORS *******************/
/**************************************************/


Text::Text()
{
  buffer = nullptr;

}


Text::~Text()
{
  if (buffer)
    free(buffer);
}


/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/

void Text::font_read(const std::string &font_file)
{
  FILE *fp = fopen(font_file.c_str(), "rb");
  if (!fp) return;
  fseek(fp, 0, SEEK_END);
  int size = ftell(fp);
  fseek(fp, 0, SEEK_SET);
        
  buffer = (unsigned char *) malloc(size); 
  if (!buffer) {
    std::cout << "Error: font file '" << font_file << "' has zero size" << std::endl;
    fclose(fp);
    return;
  }

  fread(buffer, 1, size, fp);
  fclose(fp);
}


void Text::font_bitmap_bake()
{
  unsigned char *bitmap;

  bitmap = new unsigned char[512 * 512];

  if (!buffer) {
    std::cout << "Error: no font loaded" << std::endl;
    delete bitmap;
    return;
  }

  stbtt_BakeFontBitmap(buffer, 0, 32.0, bitmap, 512, 512, 32, 96, cdata);
  Image &image = texture.image_get();
  image.data_set(bitmap, 512, 512);
}


Texture &Text::font_texture_get()
{
  return texture;
}


const glm::vec4 &Text::font_coordinates_get()
{
}


