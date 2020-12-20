#include <image/Image.hpp>

#include <pfm/pfm_input_file.hpp>
#include <pfm/pfm_output_file.hpp>

#include <pic/pic_input_file.hpp>
#include <pic/pic_output_file.hpp>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4018)
#endif
#define TINYEXR_IMPLEMENTATION
#include <tinyexr.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include <QImage>

#include <array>
#include <fstream>
#include <string_view>

namespace hdrv {

Image::Image(int w, int h, int c, Format f, std::vector<uint8_t>&& data)
  : width_(w)
  , height_(h)
  , channels_(c)
  , format_(f)
  , data_(std::move(data))
{}

Image::Image(int w, int h, int c, Format f, std::vector<uint8_t>&& data, std::vector<Layer>&& layers)
  : Image(w, h, c, f, std::move(data))
{
  layers_ = std::move(layers);
}

Image Image::makeEmpty()
{
  std::vector<uint8_t> data(1, 0);
  return Image(1, 1, 1, Byte, std::move(data));
}

float Image::value(int x, int y, int channel) const
{
  auto i = ((height() - y - 1) * width() + x) * channels() + channel;
  if (format_ == Float) {
    float result;
    memcpy(&result, data() + i * sizeof(float), sizeof(float));
    return result;
  } else {
    return (float)data_[i];
  }
}

Result<Image> Image::scaleByHalf() const
{
  // Looses some pixels for odd resolutions

  if (width_ <= 1 && height_ <= 1) {
    return Result<Image>("Image is too small for further downscaling by half.");
  }

  int newWidth = std::max(width_ / 2, 1);
  int newHeight = std::max(height_ / 2, 1);

  std::vector<uint8_t> newdata(newWidth * newHeight * channels_ * pixelSizeInBytes());
  if (format_ == Float)
  {
    for (int x = 0; x < newWidth; ++x)
      for (int y = 0; y < newHeight; ++y)
        for (int c = 0; c < channels_; ++c)
        {
          float avg = 0.f;
          for (int xo = 0; xo < 2; ++xo)
            for (int yo = 0; yo < 2; ++yo)
            {
              avg += value(
                std::min(x * 2 + xo, width_ - 1),
                std::max(height_ - y * 2 - yo - 1, 0),
                c);
            }

          const int outputOffset =
            y * channels_ * newWidth +
            x * channels_ +
            c;

          avg /= 4.f;
          memcpy(newdata.data() + outputOffset * sizeof(float), &avg, sizeof(float));
        }
  }
  else
  {
    return Result<Image>("Scaling non-floating-point images is not supported.");
  }

  return Result<Image>(Image(newWidth, newHeight, channels_, format_, std::move(newdata)));
}

// PFM

Result<Image> Image::loadPFM(std::string const& path)
{
  try {
    std::ifstream stream(path, std::ios::binary);
    return loadPFM(stream);
  }
  catch (std::exception const& e) {
    return Result<Image>(std::string("PFM loader: ") + e.what());
  }
}

Result<Image> Image::loadPFM(std::istream & stream)
{
  try {
    pfm::pfm_input_file file(stream);

    pfm::format_type format;
    size_t width, height;
    pfm::byte_order_type byteOrder;
    double scale;
    file.read_header(format, width, height, byteOrder, scale);
    int c = format == pfm::color_format ? 3 : 1;
    int w = (int)width;
    int h = (int)height;

    std::vector<uint8_t> data(w * h * c * sizeof(float));
    float* d = reinterpret_cast<float*>(data.data());
    for (int y = 0; y < h; ++y) {
      if (format == pfm::color_format) {
        file.read_color_scanline(reinterpret_cast<pfm::color_pixel *>(&d[y * w * c]), width);
      } else {
        file.read_grayscale_scanline(reinterpret_cast<pfm::grayscale_pixel *>(&d[y * w * c]), width);
      }
    }

    return Result<Image>(Image(w, h, c, Float, std::move(data)));

  } catch (std::exception const& e) {
    return Result<Image>(std::string("PFM loader: ") + e.what());
  }
}

Result<bool> Image::storePFM(std::string const& path) const
{
  if (format() != Float) {
    return Result<bool>("Cannot store LDR image as HDR image.");
  }
  try {
    std::ofstream stream(path, std::ios::binary);
    pfm::pfm_output_file file(stream);
    file.write_header(pfm::color_format, width(), height(), pfm::host_byte_order, 1.0);

    std::unique_ptr<pfm::color_pixel[]> scanline(new pfm::color_pixel[width()]);
    for (int y = height() - 1; y >= 0; --y) {
      for (int x = 0; x < width(); ++x) {
        scanline[x][0] = value(x, y, 0);
        scanline[x][1] = value(x, y, 1);
        scanline[x][2] = value(x, y, 2);
      }
      file.write_color_scanline(scanline.get(), width());
    }
    return Result<bool>(true);

  } catch (std::exception const& e) {
    return Result<bool>(std::string("PFM export failed: ") + e.what());
  }
}

// Radiance PIC

Result<Image> Image::loadPIC(std::string const& path)
{
  try {
    std::ifstream stream(path, std::ios::binary);
    return loadPIC(stream);
  }
  catch (std::exception const& e) {
    return Result<Image>(std::string("Radiance PIC loader: ") + e.what());
  }
}

Result<Image> Image::loadPIC(std::istream & stream)
{
  try {
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

    std::vector<uint8_t> data(w * h * 3 * sizeof(float));
    float* d = reinterpret_cast<float*>(data.data());
    std::unique_ptr<pic::pixel[]> scanline(new pic::pixel[w]);
    for (int y = 0; y < h; ++y) {
      file.read_scanline(scanline.get(), w);
      for (int x = 0; x < w; ++x) {
        int index = (h - y - 1) * 3 * w + x * 3;
        pic::rgbe_to_rgb(scanline[x][0], scanline[x][1], scanline[x][2], scanline[x][3],
          d[index], d[index + 1], d[index + 2]);
      }
    }

    return Result<Image>(Image(w, h, 3, Float, std::move(data)));

  } catch (std::exception const& e) {
    return Result<Image>(std::string("Radiance PIC loader: ") + e.what());
  }
}

Result<bool> Image::storePIC(std::string const& path) const
{
  if (format() != Float) {
    return Result<bool>("Cannot store LDR image as HDR image.");
  }
  try {
    std::ofstream stream(path, std::ios::binary);
    pic::pic_output_file file(stream);

    file.write_information_header(pic::_32_bit_rle_rgbe, 1.0);
    file.write_resolution_string(pic::neg_y_pos_x, width(), height());

    std::unique_ptr<pic::pixel[]> scanline(new pic::pixel[width()]);
    for (int y = 0; y < height(); ++y) {
      for (int x = 0; x < width(); ++x) {
        pic::rgb_to_rgbe(value(x, y, 0), value(x, y, 1), value(x, y, 2),
          scanline[x][0], scanline[x][1], scanline[x][2], scanline[x][3]);
      }
      file.write_scanline(scanline.get(), width());
    }
    return Result<bool>(true);

  } catch (std::exception const& e) {
    return Result<bool>(std::string("Radiance PIC export failed: ") + e.what());
  }
}

// ILM OpenEXR

#define EXR_CHECK(ret, msg) \
  if (ret != TINYEXR_SUCCESS) { \
    std::string msgString(msg); \
    if (err != nullptr) { \
      msgString += ": " + std::string(err); \
      FreeEXRErrorMessage(err); \
    } \
    return Result<Image>(msgString); \
  }

struct EXRChannel
{
  std::string_view name;
  int index = -1;

  int order() const {
    if (name == "R") return 0;
    if (name == "G") return 1;
    if (name == "B") return 2;
    if (name == "A") return 3;
    if (name == "X") return 0;
    if (name == "Y") return 1;
    if (name == "Z") return 2;
    return 0;
  }
};

inline bool operator<(EXRChannel const& a, EXRChannel const& b) {
  return a.order() < b.order();
}

struct EXRLayer
{
  std::string_view name;
  std::array<EXRChannel, 4> channels;
  int channelCount = 0;
};

Result<Image> Image::loadEXR(std::byte const* inputMemory, size_t size)
{
  auto memory = reinterpret_cast<unsigned char const*>(inputMemory);
  char const* err = nullptr;

  EXRVersion exrVersion;
  EXRImage exrImage;
  EXRHeader exrHeader;

  InitEXRHeader(&exrHeader);

  EXR_CHECK(ParseEXRVersionFromMemory(&exrVersion, memory, size),
            "Failed to parse EXR version header");

  EXR_CHECK(ParseEXRHeaderFromMemory(&exrHeader, &exrVersion, memory, size, &err),
            "Failed to parse EXR header");

  for (int i = 0; i < exrHeader.num_channels; ++i) {
    exrHeader.requested_pixel_types[i] = TINYEXR_PIXELTYPE_FLOAT;
  }

  InitEXRImage(&exrImage);
  EXR_CHECK(LoadEXRImageFromMemory(&exrImage, &exrHeader, memory, size, &err),
            "Failed to decode EXR image");

  EXRLayer defaultLayer;
  std::vector<EXRLayer> layers;
  int totalChannels = 0;

  // EXR channels are typically named A, B, G, R (ordered alphabetically).
  // However the file may also contain additional layers using dot-separated
  // naming scheme. And technically channels can have any name and occur in any order.
  for (int i = 0; i < exrHeader.num_channels; ++i) {
    std::string_view fullName = exrHeader.channels[i].name;
    std::string_view channelName = fullName;
    EXRLayer* layer = nullptr;
    if (auto n = fullName.rfind('.'); n == std::string_view::npos) {
      layer = &defaultLayer;
    } else {
      auto layerName = fullName.substr(0, n);
      channelName = fullName.substr(n + 1);
      auto compareName = [&](auto& l) { return l.name == layerName; };
      if (auto x = std::find_if(layers.begin(), layers.end(), compareName); x != layers.end()) {
        layer = &(*x);
      } else {
        layers.push_back(EXRLayer{layerName, {}, 0});
        layer = &layers.back();
      }
    }
    if (layer->channelCount < 4) {
      layer->channels[layer->channelCount++] = EXRChannel{channelName, i};
      ++totalChannels;
    }
  }

  layers.insert(layers.begin(), defaultLayer);
  for (auto& layer : layers) {
    std::sort(layer.channels.begin(), layer.channels.begin() + layer.channelCount);
  }

  // All layers are going to be stored in one big buffer. The layer structs remembers
  // the offset where a new layer starts.
  std::vector<Image::Layer> resultLayers(layers.size());
  std::vector<uint8_t> resultData(totalChannels * exrImage.width * exrImage.height * sizeof(float));
  size_t dataOffset = 0;

  for (int l = 0; l < int(layers.size()); ++l) {
    auto& layer = layers[l];
    resultLayers[l].name = layer.name;
    resultLayers[l].offset = dataOffset;

    // EXR contains one buffer per channel, so we need to convert to interlaced
    // RGBA pixel format (supporting 1-4 channels). This also does a vertical flip.
    for (int c = 0; c < layer.channelCount; ++c) {
      auto& channel = layer.channels[c];
      resultLayers[l].channels += 1;
      for (int y = 0; y < exrImage.height; ++y) {
        for (int x = 0; x < exrImage.width; ++x) {
          int coord = y * exrImage.width + x;
          int coordFlipped = (exrImage.height - y - 1) * exrImage.width + x;
          auto src = exrImage.images[channel.index] + coord * sizeof(float);
          auto dst = resultData.data() + dataOffset
            + (coordFlipped * layer.channelCount + c) * sizeof(float);
          std::memcpy(dst, src, sizeof(float));
        }
      }
    }
    dataOffset += exrImage.width * exrImage.height * layer.channelCount * sizeof(float);
    Q_ASSERT(dataOffset <= resultData.size());
  }
  Q_ASSERT(dataOffset == resultData.size());

  return Image(exrImage.width, exrImage.height, defaultLayer.channelCount, Image::Float,
               std::move(resultData), std::move(resultLayers));
}

Result<Image> Image::loadEXR(std::string const& path)
{
  std::ifstream stream(path, std::ios::in | std::ios::binary);
  stream.seekg(0, std::ios::end);
  size_t size = stream.tellg();
  stream.seekg(0, std::ios::beg);
  std::vector<std::byte> memory(size);
  stream.read(reinterpret_cast<char*>(memory.data()), size);
  if (!stream.good()) {
    return Result<Image>(std::string("Could not read file ") + path);
  }
  return loadEXR(memory.data(), size);
}

Result<bool> Image::storeEXR(std::string const& path) const
{
  try {
    int w = width();
    int h = height();

    EXRHeader header;
    InitEXRHeader(&header);

    EXRImage image;
    InitEXRImage(&image);
    image.num_channels = channels();
    image.width = width();
    image.height = height();

    float const* interlaced = reinterpret_cast<float const*>(data());
    std::vector<float> perChannel(w * h * channels());
    float* pixelPointers[4] = {};
    EXRChannelInfo channelInfos[4] = {};
    int pixelTypes[4] = {};
    int requestedPixelTypes[4] = {};

    int channelsAdded = 0;
    auto addChannel = [&](int index, char const* name) {      
      Q_ASSERT(index >= 0 && index <= 4);
      for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
          int interlacedIndex = ((h - y - 1) * w + x) * channels() + index;
          int perChannelIndex = index * w * h + y * w + x;
          Q_ASSERT(interlacedIndex < w * h * channels());
          Q_ASSERT(perChannelIndex < perChannel.size());
          perChannel[perChannelIndex] = interlaced[interlacedIndex];
        }
      }
      int d = channelsAdded++;
      pixelPointers[d] = perChannel.data() + index * w * h;
      snprintf(channelInfos[d].name, 256, name);
      pixelTypes[d] = TINYEXR_PIXELTYPE_FLOAT;
      requestedPixelTypes[d] = TINYEXR_PIXELTYPE_FLOAT;
    };

    // Add channels in alphabetical order (like OpenEXR does)
    if (channels() >= 4) {
      addChannel(3, "A");
    }
    if (channels() >= 3) {
      addChannel(2, "B");
    }
    if (channels() >= 2) {
      addChannel(1, "G");
    }
    if (channels() >= 1) {
      addChannel(0, "R");
    }
    Q_ASSERT(channelsAdded == channels());

    image.images = reinterpret_cast<unsigned char**>(pixelPointers);

    header.num_channels = channelsAdded;
    header.channels = channelInfos;
    header.pixel_types = pixelTypes;
    header.requested_pixel_types = requestedPixelTypes;

    char const* err = nullptr;
    int ret = SaveEXRImageToFile(&image, &header, path.c_str(), &err);
    if (ret != TINYEXR_SUCCESS) {
      std::string errString(err);
      FreeEXRErrorMessage(err);
      return Result<bool>(std::move(errString));
    }
    return true;

  } catch (std::exception const& e) {
    return Result<bool>(std::string("EXR export failed: ") + e.what());
  }
}

// Qt LDR Image

Result<bool> Image::storeImage(std::string const& path, float brightness, float gamma) const
{
  if (channels() == 2 || channels() > 4) return Result<bool>("Unsupported number of channels.");

  auto format = channels() == 1 ? QImage::Format_Mono :
    (channels() == 3 ? QImage::Format_RGB888 : QImage::Format_RGBA8888);

  QImage img(width(), height(), format);
  for (int y = 0; y < height(); ++y) {
    for (int x = 0; x < width(); ++x) {
      auto p = img.bits() + channels()*(y*width() + x);
      for (int c = 0; c < channels(); ++c) {
        p[c] = std::max(std::min(std::pow(brightness * value(x, y, c), gamma), 1.0f), 0.0f) * 255.0f;
      }
    }
  }
  img.save(path.c_str());

  return Result<bool>(true);
}

Result<Image> Image::loadImage(std::string const& path)
{
  QImage img;
  if (img.load(path.c_str())) {
    img = img.convertToFormat(img.hasAlphaChannel() ? QImage::Format_RGBA8888 : QImage::Format_RGB888).mirrored();

    int w = img.width();
    int h = img.height();
    int c = img.hasAlphaChannel() ? 4 : 3;

    std::vector<uint8_t> data(w * h * c);
    std::copy(img.bits(), img.bits() + w*h*c, data.begin());
    return Result<Image>(Image(w, h, c, Byte, std::move(data)));
  } else {
    return Result<Image>(std::string("Image loader failed."));
  }
}

}
