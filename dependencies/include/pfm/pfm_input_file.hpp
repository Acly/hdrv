#ifndef PFM_PFM_INPUT_FILE_HPP_INCLUDED
#define PFM_PFM_INPUT_FILE_HPP_INCLUDED

#include <istream>

#include <pfm/pfm.hpp>
#include <pfm/byte_order.hpp>
#include <pfm/color_pixel.hpp>

namespace pfm {

class pfm_input_file
{
public:
  pfm_input_file(std::istream& istream);
  void read_header(format_type& format, std::size_t& width, std::size_t& height, byte_order_type& byte_order, double& scale);
  void read_color_scanline(color_pixel* scanline, std::size_t length);
  void read_grayscale_scanline(grayscale_pixel* scanline, std::size_t length);
private:
  std::istream& istream_;
  format_type format_;
  std::size_t width_;
  std::size_t height_;
  byte_order_type byte_order_;
  double scale_;
};

} // namespace pfm

inline pfm::pfm_input_file::pfm_input_file(std::istream& istream)
  : istream_(istream)
{
}

#endif // PFM_PFM_INPUT_FILE_HPP_INCLUDED
