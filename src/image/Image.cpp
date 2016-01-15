#include <image/Image.hpp>

#include <fstream>
#include <pfm/pfm_input_file.hpp>
#include <pic/pic_input_file.hpp>

namespace hdrv {

Image::Image(int w, int h, int c, std::unique_ptr<float[]> data)
  : width_(w)
  , height_(h)
  , channels_(c)
  , data_(std::move(data))
{}

Result<Image> Image::loadPFM(std::string const& path)
{
  try {
    std::ifstream stream(path, std::ios::binary);
    pfm::pfm_input_file file(stream);

    pfm::format_type format;
    size_t width, height;
    pfm::byte_order_type byteOrder;
    double scale;
    file.read_header(format, width, height, byteOrder, scale);
    int c = format == pfm::color_format ? 3 : 1;
    int w = (int)width;
    int h = (int)height;

    std::unique_ptr<float[]> data(new float[w * h * c]);
    for (int y = 0; y < h; ++y) {
      if (format == pfm::color_format) {
        file.read_color_scanline(reinterpret_cast<pfm::color_pixel *>(&data[y * w * c]), width);
      } else {
        file.read_grayscale_scanline(reinterpret_cast<pfm::grayscale_pixel *>(&data[y * w * c]), width);
      }
    }

    return Result<Image>(Image(w, h, c, std::move(data)));

  } catch (std::exception const& e) {
    return Result<Image>(std::string("PFM loader: ") + e.what());
  }
}

Result<Image> Image::loadPIC(std::string const& path)
{
  try {
    std::ifstream stream(path, std::ios::binary);
    pic::pic_input_file file(stream);

    pic::format_type format;
    double exposure;
    file.read_information_header(format, exposure);
    if (format != pic::_32_bit_rle_rgbe) {
      return Result<Image>("Radiance PIC loader: format not supported.");
    }

    pic::resolution_string_type resolutionType;
    size_t width, height;
    file.read_resolution_string(resolutionType, width, height);
    if (resolutionType != pic::neg_y_pos_x) {
      return Result<Image>("Radiance PIC loader: resolution type not supported.");
    }
    int w = (int)width;
    int h = (int)height;

    std::unique_ptr<float[]> data(new float[w * h * 3]);

    std::unique_ptr<pic::pixel[]> scanline(new pic::pixel[w]);
    for (int y = 0; y < h; ++y) {
      file.read_scanline(scanline.get(), w);
      for (int x = 0; x < w; ++x) {
        int index = (h - y - 1) * 3 * w + x * 3;
        pic::rgbe_to_rgb(scanline[x][0], scanline[x][1], scanline[x][2], scanline[x][3],
          data[index], data[index + 1], data[index + 2]);
      }
    }

    return Result<Image>(Image(w, h, 3, std::move(data)));

  } catch (std::exception const& e) {
    return Result<Image>(std::string("Radiance PIC loader: ") + e.what());
  }
}

}