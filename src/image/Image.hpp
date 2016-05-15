#pragma once

#include <string>
#include <memory>
#include <vector>
#include <boost/optional.hpp>

namespace hdrv {

template<typename T>
class Result
{
public:
  operator bool() const { return (bool)value_; }
  T const& value() const& { return value_.get(); }
  T && value() && { return std::move(value_.get()); }
  std::string const& error() const { return error_; }

  Result(T && v) : value_(std::move(v)) {}
  Result(std::string const& error) : error_(error) {}

private:
  std::string error_;
  boost::optional<T> value_;
};

class Image
{
public:
  enum Format { Byte, Float };

  static Image makeEmpty();

  static Result<Image> loadPFM(std::string const& path);
  static Result<Image> loadPIC(std::string const& path);
  static Result<Image> loadEXR(std::string const& path);
  static Result<Image> loadImage(std::string const& path);

  int width() const { return width_; }
  int height() const { return height_; }
  int channels() const { return channels_; }
  int pixelSizeInBytes() const { return format_ == Byte ? sizeof(uint8_t) : sizeof(float); }
  int sizeInBytes() const { return width_ * height_ * channels_ * pixelSizeInBytes(); }
  Format format() const { return format_; }
  void const* data() const { return data_.data(); }

  template<typename T>
  T value(int x, int y, int channel) const
  {
    auto i = ((height() - y - 1) * width() + x) * channels() + channel;
    if (format_ == Float) {
      float const* d = reinterpret_cast<float const*>(data_.data());
      return T(d[i]);
    } else {
      uint8_t const* d = reinterpret_cast<uint8_t const*>(data_.data());
      return T(d[i]);
    } 
  }

  Result<bool> storePFM(std::string const& path) const;
  Result<bool> storePIC(std::string const& path) const;
  Result<bool> storeEXR(std::string const& path) const;

  Image(int w, int h, int c, Format f, std::vector<uint8_t>&& data);

private:
  int width_;
  int height_;
  int channels_;
  Format format_;
  std::vector<uint8_t> data_;
};

}