/*

#pragma once

#include <windows.h>

class IStreamDevice
{
public:
  typedef char char_type;

  struct category :
    virtual boost::iostreams::device_tag,
    virtual boost::iostreams::input_seekable
  {
  };

  IStreamDevice(IStream * stream)
    : stream_(stream)
  {
  }

  std::streamsize read(char_type* buffer, std::streamsize buffersize)
  {
    ULONG bytesRead = 0;
    HRESULT hr = stream_->Read(buffer, (ULONG)buffersize, &bytesRead);

    if (SUCCEEDED(hr))
    {
      return bytesRead;
    }
    else
    {
      return -1;
    }
  }

  std::streampos seek(boost::iostreams::stream_offset offset,
    std::ios_base::seekdir seekdir)
  {
    DWORD origin = 0;

    if (seekdir == std::ios_base::beg)
    {
      origin = STREAM_SEEK_SET;
    }
    else if (seekdir == std::ios_base::cur)
    {
      origin = STREAM_SEEK_CUR;
    }
    else if (seekdir == std::ios_base::end)
    {
      origin = STREAM_SEEK_END;
    }
    else
    {
      throw std::ios_base::failure("bad seek direction");
    }

    ULARGE_INTEGER newpos;
    LARGE_INTEGER off;
    off.QuadPart = offset;
    HRESULT hr = stream_->Seek(off, origin, &newpos);
    if (FAILED(hr))
    {
      throw std::ios_base::failure("bad seek offset");
    }

    return boost::iostreams::offset_to_position(newpos.QuadPart);
  }

private:
  IStream * stream_;
};

// Compatible with std::istream
typedef boost::iostreams::stream<IStreamDevice> IStreamStream;

// For loading openexr formats
class IStreamExr : public Imf::IStream
{
public:
  IStreamExr(::IStream * stream)
    : Imf::IStream("memory"), stream_(stream), currentPos_(0)
  {
    ULARGE_INTEGER newpos;
    LARGE_INTEGER off;
    off.QuadPart = 0;
    stream_->Seek(off, STREAM_SEEK_END, &newpos);
    streamSize_ = newpos.QuadPart;
    stream_->Seek(off, STREAM_SEEK_SET, nullptr);
  }

  bool read(char c[], int n) override
  {
    ULONG bytesRead = 0;
    HRESULT hr = stream_->Read(c, (ULONG)n, &bytesRead);

    if (FAILED(hr))
    {
      throw std::runtime_error("reading stream failed");
    }
    currentPos_ += bytesRead;
    return (currentPos_ != streamSize_);
  }

  Imf::Int64 tellg() override
  {
    return Imf::Int64(currentPos_);
  }

  void seekg(Imf::Int64 pos) override
  {
    LARGE_INTEGER off;
    off.QuadPart = pos;
    stream_->Seek(off, STREAM_SEEK_SET, nullptr);

    currentPos_ = pos;
  }
private:
  ::IStream * stream_;
  size_t currentPos_;
  size_t streamSize_;
};

*/
