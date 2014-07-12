#pragma once

#include <iostream>
#include <string>

class Image {

public:
  unsigned char *data;

  int width;
  int height;
  int components;

   Image();
  ~Image();

  bool load(const std::string &filename);

};

