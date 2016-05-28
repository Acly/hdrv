#include <pic/pic_input_file.hpp>

#include <cassert>
#include <cctype>
#include <cstring>
#include <sstream>

#ifdef PIC_DEBUG
#  include <iostream>
#endif

void pic::pic_input_file::read_information_header(format_type& format, double& exposure)
{
  std::string line;
  line_number_ = 0;

  // magic
  const std::size_t magic_size = 10;
  char magic[magic_size];
  istream_.read(magic, magic_size);
  if (!istream_) {
    throw pic::runtime_error(std::string("pic: error: ") + __FUNCTION__);
  }
  ++line_number_;
  if (!istream_ || strncmp(magic, "#?RADIANCE", magic_size)) {
    #ifdef PIC_DEBUG
      std::cerr << "pic: " << line_number_ << ": " << "error" << std::endl;
    #endif
    throw pic::runtime_error(std::string("pic: error: ") + __FUNCTION__);
  }
  istream_.ignore(1);
  if (!istream_) {
    #ifdef PIC_DEBUG
      std::cerr << "pic: " << line_number_ << ": " << "error" << std::endl;
    #endif
    throw pic::runtime_error(std::string("pic: error: ") + __FUNCTION__);
  }
  #ifdef PIC_DEBUG
    std::cerr << "pic: " << line_number_ << ": " << "info: " << magic << std::endl;
  #endif

  std::size_t number_of_format_lines = 0;
  double cumulative_exposure = 1.0;
  double cumulative_colorcorr[3] = {1.0, 1.0, 1.0};
  double cumulative_pixaspect = 1.0;
  std::size_t number_of_primaries_lines = 0;

  while (std::getline(istream_, line)) {
    ++line_number_;

    if (!line.empty()) {
      std::string::size_type pos = line.find('=');
      if (pos != std::string::npos) {
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1, line.size());

        // format
        if (key == "FORMAT") {
          std::string format_string;
          std::istringstream istringstream(value);
          istringstream.unsetf(std::ios_base::skipws);
          istringstream >> std::ws >> format_string >> std::ws;
          if (!istringstream || !istringstream.eof() || ((format_string != "32-bit_rle_rgbe") && (format_string != "32-bit_rle_xyze"))) {
            #ifdef PIC_DEBUG
              std::cerr << "pic: " << line_number_ << ": " << "error" << std::endl;
            #endif
            throw pic::runtime_error(std::string("pic: error: ") + __FUNCTION__);
          }
          if (number_of_format_lines > 0) {
            #ifdef PIC_DEBUG
              std::cerr << "pic: " << line_number_ << ": " << "error" << std::endl;
            #endif
            throw pic::runtime_error(std::string("pic: error: ") + __FUNCTION__);
          }
          #ifdef PIC_DEBUG
            std::cerr << "pic: " << line_number_ << ": " << "info: " << "FORMAT=" << format_string << std::endl;
          #endif
          if (format_string == "32-bit_rle_rgbe") {
            format = _32_bit_rle_rgbe;
          }
          else {
            format = _32_bit_rle_xyze;
          }
          ++number_of_format_lines;
        }

        // exposure
        else if (key == "EXPOSURE") {
          double exposure;
          std::istringstream istringstream(value);
          istringstream.unsetf(std::ios_base::skipws);
          istringstream >> std::ws >> exposure;
          if (!istringstream || !istringstream.eof() || (exposure < 0.0)) {
            #ifdef PIC_DEBUG
              std::cerr << "pic: " << line_number_ << ": " << "error" << std::endl;
            #endif
            throw pic::runtime_error(std::string("pic: error: ") + __FUNCTION__);
          }
          #ifdef PIC_DEBUG
            std::cerr << "pic: " << line_number_ << ": " << "info: " << "EXPOSURE=" << exposure << std::endl;
          #endif
          cumulative_exposure *= exposure;
        }

        // color correction
        else if (key == "COLORCORR") {
          double colorcorr[3];
          char space_colorcorr_0_colorcorr_1, space_colorcorr_1_colorcorr_2;
          std::istringstream istringstream(value);
          istringstream.unsetf(std::ios_base::skipws);
          istringstream >> std::ws >> colorcorr[0] >> space_colorcorr_0_colorcorr_1 >> std::ws >> colorcorr[1] >> space_colorcorr_1_colorcorr_2 >> std::ws >> colorcorr[2] >> std::ws;
          if (!istringstream || !istringstream.eof() || (!std::isspace(space_colorcorr_0_colorcorr_1)) || (!std::isspace(space_colorcorr_1_colorcorr_2))) {
            #ifdef PIC_DEBUG
              std::cerr << "pic: " << line_number_ << ": " << "error" << std::endl;
            #endif
            throw pic::runtime_error(std::string("pic: error: ") + __FUNCTION__);
          }
          #ifdef PIC_DEBUG
            std::cerr << "pic: " << line_number_ << ": " << "info: " << "COLORCORR=" << colorcorr[0] << " " << colorcorr[1] << " " << colorcorr[2] << std::endl;
          #endif
          cumulative_colorcorr[0] *= colorcorr[0];
          cumulative_colorcorr[1] *= colorcorr[1];
          cumulative_colorcorr[2] *= colorcorr[2];
          #ifdef PIC_DEBUG
            std::cerr << "pic: " << line_number_ << ": " << "warning: " << "ignoring line ‘" << line << "’" << std::endl;
          #endif
        }

        // software
        else if (key == "SOFTWARE") {
          std::string software = value;
          #ifdef PIC_DEBUG
            std::cerr << "pic: " << line_number_ << ": " << "info: " << "SOFTWARE=" << software << std::endl;
          #endif
          #ifdef PIC_DEBUG
            std::cerr << "pic: " << line_number_ << ": " << "warning: " << "ignoring line ‘" << line << "’" << std::endl;
          #endif
        }

        // pixel aspect ratio
        else if (key == "PIXASPECT") {
          double pixaspect;
          std::istringstream istringstream(value);
          istringstream.unsetf(std::ios_base::skipws);
          istringstream >> std::ws >> pixaspect;
          if (!istringstream || !istringstream.eof()) {
            #ifdef PIC_DEBUG
              std::cerr << "pic: " << line_number_ << ": " << "error" << std::endl;
            #endif
            throw pic::runtime_error(std::string("pic: error: ") + __FUNCTION__);
          }
          #ifdef PIC_DEBUG
            std::cerr << "pic: " << line_number_ << ": " << "info: " << "PIXASPECT=" << pixaspect << std::endl;
          #endif
          cumulative_pixaspect *= pixaspect;
          #ifdef PIC_DEBUG
            std::cerr << "pic: " << line_number_ << ": " << "warning: " << "ignoring line ‘" << line << "’" << std::endl;
          #endif
        }

        // view
        else if (key == "VIEW") {
          std::string view_parameters = value;
          #ifdef PIC_DEBUG
            std::cerr << "pic: " << line_number_ << ": " << "info: " << "VIEW=" << view_parameters << std::endl;
          #endif
          #ifdef PIC_DEBUG
            std::cerr << "pic: " << line_number_ << ": " << "warning: " << "ignoring line ‘" << line << "’" << std::endl;
          #endif
        }

        // primaries
        else if (key == "PRIMARIES") {
          double primaries[4][2];
          char space_primaries_0_0_primaries_0_1, space_primaries_0_1_primaries_1_0, space_primaries_1_0_primaries_1_1, space_primaries_1_1_primaries_2_0, space_primaries_2_0_primaries_2_1, space_primaries_2_1_primaries_3_0, space_primaries_3_0_primaries_3_1;
          std::istringstream istringstream(value);
          istringstream.unsetf(std::ios_base::skipws);
          istringstream >> std::ws >> primaries[0][0] >> space_primaries_0_0_primaries_0_1 >> std::ws >> primaries[0][1] >> space_primaries_0_1_primaries_1_0 >> std::ws >> primaries[1][0] >> space_primaries_1_0_primaries_1_1 >> std::ws >> primaries[1][1] >> space_primaries_1_1_primaries_2_0 >> std::ws >> primaries[2][0] >> space_primaries_2_0_primaries_2_1 >> std::ws >> primaries[2][1] >> space_primaries_2_1_primaries_3_0 >> std::ws >> primaries[3][0] >> space_primaries_3_0_primaries_3_1 >> std::ws >> primaries[3][1];
          if (!istringstream || !istringstream.eof() || (!std::isspace(space_primaries_0_0_primaries_0_1)) || (!std::isspace(space_primaries_0_1_primaries_1_0)) || (!std::isspace(space_primaries_1_0_primaries_1_1)) || (!std::isspace(space_primaries_1_1_primaries_2_0)) || (!std::isspace(space_primaries_2_0_primaries_2_1)) || (!std::isspace(space_primaries_2_1_primaries_3_0)) || (!std::isspace(space_primaries_3_0_primaries_3_1))) {
            #ifdef PIC_DEBUG
              std::cerr << "pic: " << line_number_ << ": " << "error" << std::endl;
            #endif
            throw pic::runtime_error(std::string("pic: error: ") + __FUNCTION__);
          }
          if (number_of_primaries_lines > 0) {
            #ifdef PIC_DEBUG
              std::cerr << "pic: " << line_number_ << ": " << "error" << std::endl;
            #endif
            throw pic::runtime_error(std::string("pic: error: ") + __FUNCTION__);
          }
          #ifdef PIC_DEBUG
            std::cerr << "pic: " << line_number_ << ": " << "info: " << "PRIMARIES=" << primaries[0][0] << " " << primaries[0][1] << " " << primaries[1][0] << " " << primaries[1][1] << " " << primaries[2][0] << " " << primaries[2][1] << " " << primaries[3][0] << " " << primaries[3][1] << std::endl;
          #endif
          ++number_of_primaries_lines;
          #ifdef PIC_DEBUG
            std::cerr << "pic: " << line_number_ << ": " << "warning: " << "ignoring line ‘" << line << "’" << std::endl;
          #endif
        }

        // unknown
        else {
          #ifdef PIC_DEBUG
            std::cerr << "pic: " << line_number_ << ": " << "warning: " << "ignoring line ‘" << line << "’" << std::endl;
          #endif
        }
      }

      // comment
      else if (line[0] == '#') {
        std::string comment = line;
        #ifdef PIC_DEBUG
          std::cerr << "pic: " << line_number_ << ": " << "info: " << comment << std::endl;
        #endif
      }

      // unknown
      else {
        #ifdef PIC_DEBUG
          std::cerr << "pic: " << line_number_ << ": " << "warning: " << "ignoring line ‘" << line << "’" << std::endl;
        #endif
      }

    }
    else {
      // empty line
      break;
    }
  }

  // format
  if (number_of_format_lines != 1) {
    #ifdef PIC_DEBUG
      std::cerr << "pic: " << line_number_ << ": " << "error" << std::endl;
    #endif
    throw pic::runtime_error(std::string("pic: error: ") + __FUNCTION__);
  }

  if (!istream_) {
    #ifdef PIC_DEBUG
      std::cerr << "pic: " << line_number_ << ": " << "error" << std::endl;
    #endif
    throw pic::runtime_error(std::string("pic: error: ") + __FUNCTION__);
  }

  exposure = cumulative_exposure;
}

void pic::pic_input_file::read_resolution_string(resolution_string_type& resolution_string_type, std::size_t& x_resolution, std::size_t& y_resolution)
{
  std::string line;

  // resolution string
  std::getline(istream_, line);
  if (!istream_) {
    #ifdef PIC_DEBUG
      std::cerr << "pic: " << line_number_ << ": " << "error" << std::endl;
    #endif
    throw pic::runtime_error(std::string("pic: error: ") + __FUNCTION__);
  }
  char sign[2];
  char dim[2];
  std::size_t resolution[2];
  char space_dim_0_resolution_0, space_resolution_0_sign_1, space_dim_1_resolution_1;
  std::istringstream istringstream(line);
  istringstream.unsetf(std::ios_base::skipws);
  istringstream >> std::ws >> sign[0] >> dim[0] >> space_dim_0_resolution_0 >> std::ws >> resolution[0] >> space_resolution_0_sign_1 >> std::ws >> sign[1] >> dim[1] >> space_dim_1_resolution_1 >> std::ws >> resolution[1] >> std::ws;
  if (!istringstream || !istringstream.eof()
    || ((sign[0] != '+') && (sign[0] != '-'))
    || ((dim[0] != 'X') && (dim[0] != 'Y'))
    || (!std::isspace(space_dim_0_resolution_0))
    || (resolution[0] == 0)
    || (!std::isspace(space_resolution_0_sign_1))
    || ((sign[1] != '+') && (sign[1] != '-'))
    || ((dim[1] != 'X') && (dim[1] != 'Y'))
    || (!std::isspace(space_dim_1_resolution_1))
    || (resolution[1] == 0)
    || (dim[0] == dim[1]))
  {
    #ifdef PIC_DEBUG
      std::cerr << "pic: " << line_number_ << ": " << "error" << std::endl;
    #endif
    throw pic::runtime_error(std::string("pic: error: ") + __FUNCTION__);
  }
  #ifdef PIC_DEBUG
    std::cerr << "pic: " << line_number_ << ": " << "info: " << sign[0] << dim[0] << " " << resolution[0] << " " << sign[1] << dim[1] << " " << resolution[1] << "\n";
  #endif
  resolution_string_type = dim[0] == 'Y' ? (sign[0] == '-' ? (sign[1] == '+' ? neg_y_pos_x : neg_y_neg_x)
                                                           : (sign[1] == '+' ? pos_y_pos_x : pos_y_neg_x))
                                         : (sign[0] == '-' ? (sign[1] == '+' ? neg_x_pos_y : neg_x_neg_y)
                                                           : (sign[1] == '+' ? pos_x_pos_y : pos_x_neg_y));
  if (dim[0] == 'Y') {
    x_resolution = resolution[1];
    y_resolution = resolution[0];
  }
  else {
    x_resolution = resolution[0];
    y_resolution = resolution[1];
  }
}

void pic::pic_input_file::read_scanline(pixel* scanline, std::size_t length)
{
  assert(scanline != 0);
  assert(length > 0);

  if ((length < 8) || (length > 0x7fff)) {
    #ifdef PIC_DEBUG
      std::cerr << istream_.tellg() << ": " << "error: " << std::endl;
    #endif
    throw pic::runtime_error(std::string("pic: error: ") + __FUNCTION__);
  }
  else {
    uint8_t header[4];
    istream_.read(reinterpret_cast<char*>(&header), 4);
    if (!istream_) {
      #ifdef PIC_DEBUG
        std::cerr << istream_.tellg() << ": " << "error: " << std::endl;
      #endif
      throw pic::runtime_error(std::string("pic: error: ") + __FUNCTION__);
    }
    if ((header[0] != 2) || (header[1] != 2)) {
      #ifdef PIC_DEBUG
        std::cerr << istream_.tellg() << ": " << "error: " << std::endl;
      #endif
      throw pic::runtime_error(std::string("pic: error: ") + __FUNCTION__);
    }
    std::size_t scanline_length = (header[2] << 8) | header[3];
    if (scanline_length != length) {
      #ifdef PIC_DEBUG
        std::cerr << istream_.tellg() << ": " << "error: " << std::endl;
      #endif
      throw pic::runtime_error(std::string("pic: error: ") + __FUNCTION__);
    }
    for (std::size_t component_index = 0; component_index < 4; ++component_index) {
      pixel* scanline_iterator = scanline;
      std::size_t bytes_left = scanline_length;
      while (bytes_left > 0) {
        // read byte
        uint8_t byte;
        istream_.read(reinterpret_cast<char*>(&byte), 1);
        if (!istream_) {
          #ifdef PIC_DEBUG
            std::cerr << istream_.tellg() << ": " << "error: " << std::endl;
          #endif
          throw pic::runtime_error(std::string("pic: error: ") + __FUNCTION__);
        }
        // run
        if ((byte > 128)) {
          std::size_t run_length = byte - 128;
          if (run_length > bytes_left) {
            #ifdef PIC_DEBUG
              std::cerr << istream_.tellg() << ": " << "error: " << std::endl;
            #endif
            throw pic::runtime_error(std::string("pic: error: ") + __FUNCTION__);
          }
          istream_.read(reinterpret_cast<char*>(&byte), 1);
          if (!istream_) {
            #ifdef PIC_DEBUG
              std::cerr << istream_.tellg() << ": " << "error: " << std::endl;
            #endif
            throw pic::runtime_error(std::string("pic: error: ") + __FUNCTION__);
          }
          for (std::size_t i = 0; i < run_length; ++i) {
            (*scanline_iterator)[component_index] = byte;
            ++scanline_iterator;
          }
          bytes_left -= run_length;
        }
        // dump
        else {
          std::size_t dump_length = byte;
          if (dump_length > bytes_left) {
            #ifdef PIC_DEBUG
              std::cerr << istream_.tellg() << ": " << "error: " << std::endl;
            #endif
            throw pic::runtime_error(std::string("pic: error: ") + __FUNCTION__);
          }
          for (std::size_t i = 0; i < dump_length; ++i) {
            istream_.read(reinterpret_cast<char*>(&byte), 1);
            if (!istream_) {
              #ifdef PIC_DEBUG
                std::cerr << istream_.tellg() << ": " << "error: " << std::endl;
              #endif
              throw pic::runtime_error(std::string("pic: error: ") + __FUNCTION__);
            }
            (*scanline_iterator)[component_index] = byte;
            ++scanline_iterator;
          }
          bytes_left -= dump_length;
        }

      }
    }
  }
}
