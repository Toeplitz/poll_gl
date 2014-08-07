#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>

class Image {
  private:
    std::vector<unsigned char> data;

public:
  int width;
  int height;
  int components;

   Image();
  ~Image();

  bool                 load(const std::string &filename);
  void                 data_copy(unsigned char *buf, const int width, const int height);
  const unsigned char &data_get() const;

};

