#ifndef PIC_PIC_OUTPUT_FILE_HPP_INCLUDED
#define PIC_PIC_OUTPUT_FILE_HPP_INCLUDED

#include <ostream>

#include <pic/pic.hpp>
#include <pic/pixel.hpp>

namespace pic {

class pic_output_file
{
public:
  pic_output_file(std::ostream& ostream);
  void write_information_header(format_type format, double exposure);
  void write_resolution_string(resolution_string_type resolution_string_type, std::size_t x_resolution, std::size_t y_resolution);
  void write_scanline(const pixel* scanline, std::size_t length);
private:
  std::ostream& ostream_;
};

} // namespace pic

inline pic::pic_output_file::pic_output_file(std::ostream& ostream)
  : ostream_(ostream)
{
}

#endif // PIC_PIC_OUTPUT_FILE_HPP_INCLUDED
