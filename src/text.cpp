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
  auto bitmap = std::unique_ptr<unsigned char [] >(new unsigned char[512 * 512]);

  if (!buffer) {
    std::cout << "Error: no font loaded" << std::endl;
    return;
  }

  stbtt_BakeFontBitmap(buffer, 0, 20.0, bitmap.get(), 512, 512, 32, 96, cdata);
  Image &image = texture.image_get();
  image.data_copy(bitmap.get(), 512, 512);
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




void Text::bake(Mesh *mesh, float x, float y) 
{
  char *text = new char[input.size() + 1];
  std::copy(input.begin(), input.end(), text);
  text[input.size()] = '\0'; 
  char *ptr = text;

  mesh->positions_clear();
  mesh->texture_st_clear();

  while (*text) {
    int c = (unsigned char) *text;

    if (c >= 32 && c < 128) {

      stbtt_aligned_quad q;
      baked_quad_get(cdata, 512, 512, c - 32, &x, &y, &q);

      mesh->positions_add(glm::vec3(q.x0, q.y0, 0));
      mesh->positions_add(glm::vec3(q.x1, q.y1, 0));
      mesh->positions_add(glm::vec3(q.x1, q.y0, 0));
      mesh->positions_add(glm::vec3(q.x0, q.y0, 0));
      mesh->positions_add(glm::vec3(q.x0, q.y1, 0));
      mesh->positions_add(glm::vec3(q.x1, q.y1, 0));

      mesh->texture_st_add(glm::vec2(q.s0, q.t0));
      mesh->texture_st_add(glm::vec2(q.s1, q.t1));
      mesh->texture_st_add(glm::vec2(q.s1, q.t0));
      mesh->texture_st_add(glm::vec2(q.s0, q.t0));
      mesh->texture_st_add(glm::vec2(q.s0, q.t1));
      mesh->texture_st_add(glm::vec2(q.s1, q.t1));
    }
    ++text;
  }

  
  this->input = input;
  delete [] ptr;
}


Font *Text::font_get()
{
  return font;
}


void Text::font_set(Font *font)
{
  this->font = font;
}


void Text::string_append(const std::string s)
{
  this->input = input + s;
}


void Text::string_erase_last()
{
  input = input.substr(0, input.size() - 1);
}


std::string &Text::string_get()
{
  return input;
}


unsigned int Text::string_len()
{
  return input.size();
}


void Text::string_set(std::string input)
{
  this->input = input;
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
