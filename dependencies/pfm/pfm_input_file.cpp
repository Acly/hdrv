#include <pfm/pfm_input_file.hpp>

#include <cassert>
#include <cctype>
#include <sstream>

#ifdef PFM_DEBUG
#  include <iostream>
#endif

void pfm::pfm_input_file::read_header(format_type& format, std::size_t& width, std::size_t& height, byte_order_type& byte_order, double& scale)
{
  std::size_t line_number = 1;
  char whitespace;

  // identifier
  const std::size_t identifier_size = 2;
  char identifier[identifier_size];
  istream_.read(identifier, identifier_size);
  if (!istream_) {
    throw pfm::runtime_error(std::string("pfm: error: ") + __FUNCTION__);
  }
  if (!istream_ || (identifier[0] != 'P') || ((identifier[1] != 'F') && (identifier[1] != 'f'))) {
    #ifdef PFM_DEBUG
      std::cerr << "pfm: " << line_number << ": " << "error" << std::endl;
    #endif
    throw pfm::runtime_error(std::string("pfm: error: ") + __FUNCTION__);
  }
  #ifdef PFM_DEBUG
    std::cerr << "pfm: " << line_number << ": " << "info: " << std::string(identifier, 2) << std::endl;
  #endif
  format_ = identifier[1] == 'F' ? color_format : grayscale_format;

  // whitespace
  istream_.get(whitespace);
  if (!istream_ || !std::isspace(whitespace)) {
    #ifdef PFM_DEBUG
      std::cerr << "pfm: " << line_number << ": " << "error" << std::endl;
    #endif
    throw pfm::runtime_error(std::string("pfm: error: ") + __FUNCTION__);
  }
  if (whitespace == '\n') {
    ++line_number;
  }

  // width
  istream_ >> width_;
  if (!istream_ || (width_ == 0)) {
    #ifdef PFM_DEBUG
      std::cerr << "pfm: " << line_number << ": " << "error" << std::endl;
    #endif
    throw pfm::runtime_error(std::string("pfm: error: ") + __FUNCTION__);
  }
  #ifdef PFM_DEBUG
    std::cerr << "pfm: " << line_number << ": " << "info: " << width_ << std::endl;
  #endif

  // blank
  char blank;
  istream_.get(blank);
  if (!istream_ || (blank != ' ')) {
    #ifdef PFM_DEBUG
      std::cerr << "pfm: " << line_number << ": " << "error" << std::endl;
    #endif
    throw pfm::runtime_error(std::string("pfm: error: ") + __FUNCTION__);
  }

  // height
  istream_ >> height_;
  if (!istream_ || (height_ == 0)) {
    #ifdef PFM_DEBUG
      std::cerr << "pfm: " << line_number << ": " << "error" << std::endl;
    #endif
    throw pfm::runtime_error(std::string("pfm: error: ") + __FUNCTION__);
  }
  #ifdef PFM_DEBUG
    std::cerr << "pfm: " << line_number << ": " << "info: " << height_ << std::endl;
  #endif

  // whitespace
  istream_.get(whitespace);
  if (!istream_ || !std::isspace(whitespace)) {
    #ifdef PFM_DEBUG
      std::cerr << "pfm: " << line_number << ": " << "error" << std::endl;
    #endif
    throw pfm::runtime_error(std::string("pfm: error: ") + __FUNCTION__);
  }
  if (whitespace == '\n') {
    ++line_number;
  }

  // byte_order and scale
  double byte_order_and_scale;
  istream_ >> byte_order_and_scale;
  if (!istream_ || (byte_order_and_scale == 0.0)) {
    #ifdef PFM_DEBUG
      std::cerr << "pfm: " << line_number << ": " << "error" << std::endl;
    #endif
    throw pfm::runtime_error(std::string("pfm: error: ") + __FUNCTION__);
  }
  #ifdef PFM_DEBUG
    std::cerr << "pfm: " << line_number << ": " << "info: " << byte_order_and_scale << std::endl;
  #endif
  if (byte_order_and_scale < 0.0) {
    byte_order_ = little_endian_byte_order;
    scale_ = -byte_order_and_scale;
  }
  else {
    byte_order_ = big_endian_byte_order;
    scale_ = byte_order_and_scale;
  }

  // whitespace
  istream_.get(whitespace);
  if (!istream_ || !std::isspace(whitespace)) {
    #ifdef PFM_DEBUG
      std::cerr << "pfm: " << line_number << ": " << "error" << std::endl;
    #endif
    throw pfm::runtime_error(std::string("pfm: error: ") + __FUNCTION__);
  }
  if (whitespace == '\n') {
    ++line_number;
  }

  format = format_;
  width = width_;
  height = height_;
  byte_order = byte_order_;
  scale = scale_;
}

void pfm::pfm_input_file::read_color_scanline(color_pixel* scanline, std::size_t length)
{
  assert(format_ == color_format);
  assert(scanline != 0);
  assert(length == width_);

  color_pixel* scanline_begin = scanline;
  color_pixel* scanline_end = scanline_begin + length;
  for (color_pixel* scanline_iterator = scanline_begin; scanline_iterator != scanline_end; ++scanline_iterator) {
    color_pixel& pixel = *scanline_iterator;
    istream_.read(reinterpret_cast<char*>(&pixel[0]), 4);
    if (!istream_) {
      #ifdef PFM_DEBUG
        std::cerr << istream_.tellg() << ": " << "error: " << std::endl;
      #endif
      throw pfm::runtime_error(std::string("pfm: error: ") + __FUNCTION__);
    }
    if (byte_order_ != host_byte_order) {
      swap_byte_order(pixel[0]);
    }
    istream_.read(reinterpret_cast<char*>(&pixel[1]), 4);
    if (!istream_) {
      #ifdef PFM_DEBUG
        std::cerr << istream_.tellg() << ": " << "error: " << std::endl;
      #endif
      throw pfm::runtime_error(std::string("pfm: error: ") + __FUNCTION__);
    }
    if (byte_order_ != host_byte_order) {
      swap_byte_order(pixel[1]);
    }
    istream_.read(reinterpret_cast<char*>(&pixel[2]), 4);
    if (!istream_) {
      #ifdef PFM_DEBUG
        std::cerr << istream_.tellg() << ": " << "error: " << std::endl;
      #endif
      throw pfm::runtime_error(std::string("pfm: error: ") + __FUNCTION__);
    }
    if (byte_order_ != host_byte_order) {
      swap_byte_order(pixel[2]);
    }
  }
}

void pfm::pfm_input_file::read_grayscale_scanline(grayscale_pixel* scanline, std::size_t length)
{
  assert(format_ == grayscale_format);
  assert(scanline != 0);
  assert(length == width_);

  grayscale_pixel* scanline_begin = scanline;
  grayscale_pixel* scanline_end = scanline_begin + length;
  for (grayscale_pixel* scanline_iterator = scanline_begin; scanline_iterator != scanline_end; ++scanline_iterator) {
    grayscale_pixel& pixel = *scanline_iterator;
    istream_.read(reinterpret_cast<char*>(&pixel), 4);
    if (!istream_) {
      #ifdef PFM_DEBUG
        std::cerr << istream_.tellg() << ": " << "error: " << std::endl;
      #endif
      throw pfm::runtime_error(std::string("pfm: error: ") + __FUNCTION__);
    }
    if (byte_order_ != host_byte_order) {
      swap_byte_order(pixel);
    }
  }
}
