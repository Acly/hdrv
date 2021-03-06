#pragma once

#include <string>
#include <memory>
#include <vector>
#include <optional>
#include <cstddef>

namespace hdrv {

template<typename T>
class Result
{
public:
  operator bool() const { return (bool)value_; }
  T const& value() const& { return value_.value(); }
  T && value() && { return std::move(value_.value()); }
  std::string const& error() const { return error_; }

  Result(T && v) : value_(std::move(v)) {}
  Result(std::string const& error) : error_(error) {}

private:
  std::string error_;
  std::optional<T> value_;
};

class Image
{
public:
  enum Format { Byte, Float };
  enum Display { Color, Luminance, Depth, Normal, Integer };

  struct Layer {
    std::string name;
    int channels;
    Display display;
    size_t offset;
  };

  static Image makeEmpty();

  static Result<Image> loadPFM(std::string const& path);
  static Result<Image> loadPIC(std::string const& path);
  static Result<Image> loadEXR(std::string const& path);
  static Result<Image> loadImage(std::string const& path);

  static Result<Image> loadPFM(std::istream& stream);
  static Result<Image> loadPIC(std::istream& stream);
  static Result<Image> loadEXR(std::byte const*, size_t);

  int width() const { return width_; }
  int height() const { return height_; }
  int channels(int layer = 0) const { return layer == 0 ? channels_ : layers_[layer].channels; }
  int pixelSizeInBytes() const { return format_ == Byte ? sizeof(uint8_t) : sizeof(float); }
  int sizeInBytes() const { return width_ * height_ * channels_ * pixelSizeInBytes(); }
  Format format() const { return format_; }
  uint8_t const* data() const { return data_.data(); }
  float value(int x, int y, int channel, int layer = 0) const;

  std::vector<Layer> const& layers() const { return layers_; }

  Result<bool> storePFM(std::string const& path) const;
  Result<bool> storePIC(std::string const& path) const;
  Result<bool> storeEXR(std::string const& path) const;
  Result<bool> storeImage(std::string const& path, float brightness, float gamma) const;

  Result<Image> scaleByHalf() const;

  Image(int w, int h, int c, Format f, std::vector<uint8_t>&& data);
  Image(int w, int h, Format f, std::vector<uint8_t>&& data, std::vector<Layer>&& layers);

private:

  int width_;
  int height_;
  int channels_;
  Format format_;
  std::vector<uint8_t> data_;
  std::vector<Layer> layers_;
};

}