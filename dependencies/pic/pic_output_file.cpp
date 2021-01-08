#include <pic/pic_output_file.hpp>

#include <cassert>

#ifdef PIC_DEBUG
#  include <iostream>
#endif

void pic::pic_output_file::write_information_header(format_type format, double exposure)
{
  assert((format == pic::_32_bit_rle_rgbe) || (format == pic::_32_bit_rle_xyze));
  assert(exposure >= 0.0);

  ostream_ << "#?RADIANCE" << "\n";
  switch (format) {
    case pic::_32_bit_rle_rgbe:
      ostream_ << "FORMAT=32-bit_rle_rgbe\n";
      break;
    case pic::_32_bit_rle_xyze:
      ostream_ << "FORMAT=32-bit_rle_xyze\n";
      break;
  }
  ostream_ << "EXPOSURE=" << exposure << "\n";
  ostream_ << "\n";

  if (!ostream_) {
    #ifdef PIC_DEBUG
      std::cerr << ostream_.tellp() << ": " << "error: " << std::endl;
    #endif
    throw pic::runtime_error(std::string("pic: error: ") + __FUNCTION__);
  }
}

void pic::pic_output_file::write_resolution_string(resolution_string_type resolution_string_type, std::size_t x_resolution, std::size_t y_resolution)
{
  assert((resolution_string_type == pic::pos_x_pos_y)
      || (resolution_string_type == pic::pos_x_neg_y)
      || (resolution_string_type == pic::neg_x_pos_y)
      || (resolution_string_type == pic::neg_x_neg_y)
      || (resolution_string_type == pic::pos_y_pos_x)
      || (resolution_string_type == pic::pos_y_neg_x)
      || (resolution_string_type == pic::neg_y_pos_x)
      || (resolution_string_type == pic::neg_y_neg_x)
  );
  assert(x_resolution > 0);
  assert(y_resolution > 0);

  switch (resolution_string_type) {
    case pic::pos_x_pos_y:
      ostream_ << "+X " << x_resolution << " +Y " << y_resolution << "\n";
      break;
    case pic::pos_x_neg_y:
      ostream_ << "+X " << x_resolution << " -Y " << y_resolution << "\n";
      break;
    case pic::neg_x_pos_y:
      ostream_ << "-X " << x_resolution << " +Y " << y_resolution << "\n";
      break;
    case pic::neg_x_neg_y:
      ostream_ << "-X " << x_resolution << " -Y " << y_resolution << "\n";
      break;
    case pic::pos_y_pos_x:
      ostream_ << "+Y " << y_resolution << " +X " << x_resolution << "\n";
      break;
    case pic::pos_y_neg_x:
      ostream_ << "+Y " << y_resolution << " -X " << x_resolution << "\n";
      break;
    case pic::neg_y_pos_x:
      ostream_ << "-Y " << y_resolution << " +X " << x_resolution << "\n";
      break;
    case pic::neg_y_neg_x:
      ostream_ << "-Y " << y_resolution << " -X " << x_resolution << "\n";
      break;
    default:
      assert(false);
  }

  if (!ostream_) {
    #ifdef PIC_DEBUG
      std::cerr << ostream_.tellp() << ": " << "error: " << std::endl;
    #endif
    throw pic::runtime_error(std::string("pic: error: ") + __FUNCTION__);
  }
}

void pic::pic_output_file::write_scanline(const pixel* scanline, std::size_t length)
{
  assert(scanline != 0);
  assert(length > 0);

  if (length < 8) {
    throw pic::runtime_error(std::string("Image width must be 8 or larger"));
  }
  if (length > 0x7fff) {
    throw pic::runtime_error(std::string("Maximum image width exceeded"));
  }
  uint8_t header[4];
  header[0] = header[1] = 2;
  header[2] = length >> 8;
  header[3] = length & 0xFF;
  ostream_.write(reinterpret_cast<const char*>(&header), 4);
  if (!ostream_) {
    #ifdef PIC_DEBUG
      std::cerr << ostream_.tellp() << ": " << "error: " << std::endl;
    #endif
    throw pic::runtime_error(std::string("pic: error: ") + __FUNCTION__);
  }
  for (std::size_t component_index = 0; component_index < 4; ++component_index) {
    const pixel* scanline_begin = scanline;
    const pixel* scanline_end = scanline + length;
    const pixel* scanline_iterator = scanline_begin;
    while (scanline_iterator != scanline_end) {
      // try to find a run [run_begin, run_end) with length run_length, at least 2 and at most 127
      const pixel* run_begin = scanline_iterator;
      const pixel* run_end;
      std::size_t run_length;
      do {
        run_end = run_begin + 1;
        run_length = 1;
        while ((run_end != scanline_end) && ((*run_end)[component_index] == (*run_begin)[component_index]) && (run_length < 127)) {
          ++run_end;
          ++run_length;
        }
        if ((run_length < 2) && (run_end != scanline_end)) {
          run_begin = run_end;
        }
      } while ((run_length < 2) && (run_end != scanline_end));
      // dump
      const pixel* dump_begin = scanline_iterator;
      const pixel* dump_end = run_begin;
      if (run_length < 2) {
        dump_end = run_end;
      }
      std::size_t dump_length = dump_end - dump_begin;
      if (dump_length > 0) {
        while (dump_length > 128) {
          uint8_t byte = 128;
          ostream_.write(reinterpret_cast<const char*>(&byte), 1);
          if (!ostream_) {
            #ifdef PIC_DEBUG
              std::cerr << ostream_.tellp() << ": " << "error: " << std::endl;
            #endif
            throw pic::runtime_error(std::string("pic: error: ") + __FUNCTION__);
          }
          for (std::size_t i = 0; i < 128; ++i) {
            ostream_.write(reinterpret_cast<const char*>(&((*(dump_begin + i))[component_index])), 1);
            if (!ostream_) {
              #ifdef PIC_DEBUG
                std::cerr << ostream_.tellp() << ": " << "error: " << std::endl;
              #endif
              throw pic::runtime_error(std::string("pic: error: ") + __FUNCTION__);
            }
          }
          dump_length -= 128;
          dump_begin += 128;
          scanline_iterator += 128;
        }
        if (dump_length > 0) {
          uint8_t byte = dump_length;
          ostream_.write(reinterpret_cast<const char*>(&byte), 1);
          if (!ostream_) {
            #ifdef PIC_DEBUG
              std::cerr << ostream_.tellp() << ": " << "error: " << std::endl;
            #endif
            throw pic::runtime_error(std::string("pic: error: ") + __FUNCTION__);
          }
          for (std::size_t i = 0; i < dump_length; ++i) {
            ostream_.write(reinterpret_cast<const char*>(&((*(dump_begin + i))[component_index])), 1);
            if (!ostream_) {
              #ifdef PIC_DEBUG
                std::cerr << ostream_.tellp() << ": " << "error: " << std::endl;
              #endif
              throw pic::runtime_error(std::string("pic: error: ") + __FUNCTION__);
            }
          }
          scanline_iterator += dump_length;
        }
      }
      // run
      if (run_length >= 2) {
        uint8_t byte = 128 + run_length;
        ostream_.write(reinterpret_cast<const char*>(&byte), 1);
        if (!ostream_) {
          #ifdef PIC_DEBUG
            std::cerr << ostream_.tellp() << ": " << "error: " << std::endl;
          #endif
          throw pic::runtime_error(std::string("pic: error: ") + __FUNCTION__);
        }
        ostream_.write(reinterpret_cast<const char*>(&((*run_begin)[component_index])), 1);
        if (!ostream_) {
          #ifdef PIC_DEBUG
            std::cerr << ostream_.tellp() << ": " << "error: " << std::endl;
          #endif
          throw pic::runtime_error(std::string("pic: error: ") + __FUNCTION__);
        }
        scanline_iterator += run_length;
      }
    }
  }
}
