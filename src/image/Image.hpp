#pragma once

#include <string>
#include <memory>
#include <boost/optional.hpp>

namespace hdrv {

template<typename T>
class Result
{
public:
  operator bool() const { return (bool)value_; }
  T const& value() const& { return value_.get(); }
  T && value() && { return std::move(value_.get()); }
  std::string const& error() { return error_; }

  Result(T && v) : value_(std::move(v)) {}
  Result(std::string const& error) : error_(error) {}

private:
  std::string error_;
  boost::optional<T> value_;
};

class Image
{
public:
  static Result<Image> loadPFM(std::string const& path);
  static Result<Image> loadPIC(std::string const& path);
  static Result<Image> loadEXR(std::string const& path);

  int width() const { return width_; }
  int height() const { return height_; }
  int channels() const { return channels_; }
  int sizeInBytes() const { return width_ * height_ * channels_ * sizeof(float); }
  float const* data() const { return data_.get(); }

  void storePFM(std::string const& path);
  void storePIC(std::string const& path);
  void storeEXR(std::string const& path);

  Image(int w, int h, int c, std::unique_ptr<float[]> data);

private:
  int width_;
  int height_;
  int channels_;
  std::unique_ptr<float[]> data_;
};

}