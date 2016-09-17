#include <pfm/pfm_output_file.hpp>

#include <cassert>

#ifdef PFM_DEBUG
#  include <iostream>
#endif

void pfm::pfm_output_file::write_header(format_type format, std::size_t width, std::size_t height, byte_order_type byte_order, double scale)
{
  assert((format == color_format) || (format == grayscale_format));
  assert(width > 0);
  assert(height > 0);
  assert((byte_order == little_endian_byte_order) || (byte_order == big_endian_byte_order));
  assert(scale > 0.0);

  ostream_ << (format == color_format ? "PF" : "Pf") << "\n";
  ostream_ << width << " " << height << "\n";
  ostream_ << (byte_order == big_endian_byte_order ? scale : -scale) << "\n";

  if (!ostream_) {
    #ifdef PFM_DEBUG
      std::cerr << ostream_.tellp() << ": " << "error: " << std::endl;
    #endif
    throw pfm::runtime_error(std::string("pfm: error: ") + __FUNCTION__);
  }

  format_ = format;
  width_ = width;
  height_ = height;
  byte_order_ = byte_order;
  scale_ = scale;
}

void pfm::pfm_output_file::write_color_scanline(const color_pixel* scanline, std::size_t length)
{
  assert(format_ == color_format);
  assert(scanline != 0);
  assert(length == width_);

  const color_pixel* scanline_begin = scanline;
  const color_pixel* scanline_end = scanline_begin + length;
  for (const color_pixel* scanline_iterator = scanline_begin; scanline_iterator != scanline_end; ++scanline_iterator) {
    color_pixel pixel = *scanline_iterator;
    if (byte_order_ != host_byte_order) {
      swap_byte_order(pixel[0]);
    }
    ostream_.write(reinterpret_cast<const char*>(&pixel[0]), 4);
    if (!ostream_) {
      #ifdef PFM_DEBUG
        std::cerr << istream_.tellp() << ": " << "error: " << std::endl;
      #endif
      throw pfm::runtime_error(std::string("pfm: error: ") + __FUNCTION__);
    }
    if (byte_order_ != host_byte_order) {
      swap_byte_order(pixel[1]);
    }
    ostream_.write(reinterpret_cast<const char*>(&pixel[1]), 4);
    if (!ostream_) {
      #ifdef PFM_DEBUG
        std::cerr << istream_.tellp() << ": " << "error: " << std::endl;
      #endif
      throw pfm::runtime_error(std::string("pfm: error: ") + __FUNCTION__);
    }
    if (byte_order_ != host_byte_order) {
      swap_byte_order(pixel[2]);
    }
    ostream_.write(reinterpret_cast<const char*>(&pixel[2]), 4);
    if (!ostream_) {
      #ifdef PFM_DEBUG
        std::cerr << istream_.tellp() << ": " << "error: " << std::endl;
      #endif
      throw pfm::runtime_error(std::string("pfm: error: ") + __FUNCTION__);
    }
  }
}

void pfm::pfm_output_file::write_grayscale_scanline(const grayscale_pixel* scanline, std::size_t length)
{
  assert(format_ == grayscale_format);
  assert(scanline != 0);
  assert(length == width_);

  const grayscale_pixel* scanline_begin = scanline;
  const grayscale_pixel* scanline_end = scanline_begin + length;
  for (const grayscale_pixel* scanline_iterator = scanline_begin; scanline_iterator != scanline_end; ++scanline_iterator) {
    grayscale_pixel pixel = *scanline_iterator;
    if (byte_order_ != host_byte_order) {
      swap_byte_order(pixel);
    }
    ostream_.write(reinterpret_cast<char*>(&pixel), 4);
    if (!ostream_) {
      #ifdef PFM_DEBUG
        std::cerr << istream_.tellp() << ": " << "error: " << std::endl;
      #endif
      throw pfm::runtime_error(std::string("pfm: error: ") + __FUNCTION__);
    }
  }
}
