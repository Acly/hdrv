#ifndef PIC_PIC_INPUT_FILE_HPP_INCLUDED
#define PIC_PIC_INPUT_FILE_HPP_INCLUDED

#include <istream>

#include <pic/pic.hpp>
#include <pic/pixel.hpp>

namespace pic {

class pic_input_file
{
public:
  pic_input_file(std::istream& istream);
  void read_information_header(format_type& format, double& exposure);
  void read_resolution_string(resolution_string_type& resolution_string_type, std::size_t& x_resolution, std::size_t& y_resolution);
  void read_scanline(pixel* scanline, std::size_t length);
private:
  std::istream& istream_;
  std::size_t line_number_;
};

} // namespace pic

inline pic::pic_input_file::pic_input_file(std::istream& istream)
  : istream_(istream)
{
}

#endif // PIC_PIC_INPUT_FILE_HPP_INCLUDED
