#include "text.h"
#include <stdio.h>
#include <vector>

#define STB_TRUETYPE_IMPLEMENTATION 
#include "../external/stb_truetype.h"

static stbtt_bakedchar cdata[96];

static void baked_quad_get(stbtt_bakedchar *chardata, int pw, int ph, int char_index,
                           float *xpos, float *ypos, stbtt_aligned_quad *q);


/**************************************************/
/***************** FONT CLASS *********************/
/**************************************************/

/**************************************************/
/***************** CONSTRUCTORS *******************/
/**************************************************/

Font::Font()
{
  buffer = nullptr;
}

Font::~Font()
{
  if (buffer)
    free(buffer);
}


/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/


void Font::bake()
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


void Font::load(const std::string &filename)
{
  FILE *fp = fopen(filename.c_str(), "rb");
  if (!fp) return;
  fseek(fp, 0, SEEK_END);
  int size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  buffer = (unsigned char *) malloc(size); 
  if (!buffer) {
    std::cout << "Error: font file '" << filename << "' has zero size" << std::endl;
    fclose(fp);
    return;
  }

  fread(buffer, 1, size, fp);
  fclose(fp);
}


Texture &Font::texture_get()
{
  return texture;
}


/**************************************************/
/***************** TEXT CLASS *********************/
/**************************************************/

/**************************************************/
/***************** CONSTRUCTORS *******************/
/**************************************************/


Text::Text()
{
}


Text::~Text()
{
}


/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/


const std::vector<glm::vec4> Text::bake_coords(float x, float y, char *text) 
{
  std::vector<glm::vec4> coords;

  while (*text) {
    int c = (unsigned char) *text;

    if (c >= 32 && c < 128) {

      stbtt_aligned_quad q;
      baked_quad_get(cdata, 512, 512, c - 32, &x, &y, &q);

      coords.push_back(glm::vec4(q.x0, q.y0, q.s0, q.t0));
      coords.push_back(glm::vec4(q.x1, q.y1, q.s1, q.t1));
      coords.push_back(glm::vec4(q.x1, q.y0, q.s1, q.t0));
      coords.push_back(glm::vec4(q.x0, q.y0, q.s0, q.t0));
      coords.push_back(glm::vec4(q.x0, q.y1, q.s0, q.t1));
      coords.push_back(glm::vec4(q.x1, q.y1, q.s1, q.t1));
 //     stbtt_GetBakedQuad(cdata, 512, 512, *text - 32, &x, &y, &q, 1);
      std::cout << "char: " << c << std::endl;
      std::cout << "x, y = " << x << ", " << y << std::endl;
    }
    ++text;
  }

  return coords;
}


void Text::font_set(Font *font)
{
  this->font = font;
}


/**************************************************/
/*************** STATIC FUNCTIONS *****************/
/**************************************************/


static void baked_quad_get(stbtt_bakedchar *chardata, int pw, int ph, int char_index,
                           float *xpos, float *ypos, stbtt_aligned_quad *q)
{
  stbtt_bakedchar *b = chardata + char_index;
  int round_x = STBTT_ifloor(*xpos + b->xoff);
  int round_y = STBTT_ifloor(*ypos - b->yoff);

  q->x0 = (float)round_x;
  q->y0 = (float)round_y;
  q->x1 = (float)round_x + b->x1 - b->x0;
  q->y1 = (float)round_y - b->y1 + b->y0;

  q->s0 = b->x0 / (float)pw;
  q->t0 = b->y0 / (float)pw;
  q->s1 = b->x1 / (float)ph;
  q->t1 = b->y1 / (float)ph;

  *xpos += b->xadvance;
}
