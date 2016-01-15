#ifndef PFM_PFM_OUTPUT_FILE_HPP_INCLUDED
#define PFM_PFM_OUTPUT_FILE_HPP_INCLUDED

#include <ostream>

#include <pfm/pfm.hpp>
#include <pfm/byte_order.hpp>
#include <pfm/color_pixel.hpp>

namespace pfm {

class pfm_output_file
{
public:
  pfm_output_file(std::ostream& ostream);
  void write_header(format_type format, std::size_t width, std::size_t height, byte_order_type byte_order, double scale);
  void write_color_scanline(const color_pixel* scanline, std::size_t length);
  void write_grayscale_scanline(const grayscale_pixel* scanline, std::size_t length);
private:
  std::ostream& ostream_;
  format_type format_;
  std::size_t width_;
  std::size_t height_;
  byte_order_type byte_order_;
  double scale_;
};

} // namespace pfm

inline pfm::pfm_output_file::pfm_output_file(std::ostream& ostream)
  : ostream_(ostream)
{
}

#endif // PFM_PFM_OUTPUT_FILE_HPP_INCLUDED
