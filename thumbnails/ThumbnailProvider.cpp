#include "ThumbnailProvider.hpp"
#include "Thumbnails.hpp"
#include "StreamWrapper.hpp"

#include <image/Image.hpp>
#include <sstream>
#include <math.h>
#include <algorithm>

#include <Shlwapi.h>

// Why Microsoft, why?
#ifdef min
# undef min
#endif

#pragma comment(lib, "Shlwapi.lib")

ThumbnailProvider::ThumbnailProvider(ImageExtension ext)
  : referenceCounter_(1), stream_(nullptr), imageExtension_(ext)
{
  ++ThumbnailsDll::dllReferenceCounter;
}

ThumbnailProvider::~ThumbnailProvider()
{
  --ThumbnailsDll::dllReferenceCounter;
}

// IUnknown

#pragma warning(push)
#pragma warning(disable: 4838) // conversion from 'DWORD' to 'int' requires a narrowing conversion

IFACEMETHODIMP ThumbnailProvider::QueryInterface(REFIID riid, void **ppv)
{
  static const QITAB qit[] =
  {
      QITABENT(ThumbnailProvider, IThumbnailProvider),
      QITABENT(ThumbnailProvider, IInitializeWithStream),
      { 0 },
  };
  return QISearch(this, qit, riid, ppv);
}

#pragma warning(pop)

IFACEMETHODIMP_(ULONG) ThumbnailProvider::AddRef()
{
  return ++referenceCounter_;
}

IFACEMETHODIMP_(ULONG) ThumbnailProvider::Release()
{
  ULONG cRef = --referenceCounter_;
  if (0 == cRef)
  {
    delete this;
  }

  return cRef;
}

// IInitializeWithStream

IFACEMETHODIMP ThumbnailProvider::Initialize(IStream *stream, DWORD /*grfMode*/)
{
  // A handler instance should be initialized only once in its lifetime. 
  HRESULT hr = HRESULT_FROM_WIN32(ERROR_ALREADY_INITIALIZED);
  if (stream_ == nullptr)
  {
    // Take a reference to the stream if it has not been initialized yet.
    hr = stream->QueryInterface(&stream_);
  }
  return hr;
}

// IThumbnailProvider

// Gets a thumbnail image and alpha type. The GetThumbnail is called with the 
// largest desired size of the image, in pixels. Although the parameter is 
// called cx, this is used as the maximum size of both the x and y dimensions. 
// If the retrieved thumbnail is not square, then the longer axis is limited 
// by cx and the aspect ratio of the original image respected. On exit, 
// GetThumbnail provides a handle to the retrieved image. It also provides a 
// value that indicates the color format of the image and whether it has 
// valid alpha information.
IFACEMETHODIMP ThumbnailProvider::GetThumbnail(UINT cx, HBITMAP *phbmp,
  WTS_ALPHATYPE *pdwAlpha)
{
  HRESULT hr = E_OUTOFMEMORY;

  hdrv::Result<hdrv::Image> img = hdrv::Result<hdrv::Image>("Unsupported image format");

  // Load the image from the stream
  switch (imageExtension_)
  {
  case ImageExtension::EXR: { IStreamExr s(stream_); img = hdrv::Image::loadEXR(s); break; }
  case ImageExtension::PFM: { IStreamStream s(stream_); img = hdrv::Image::loadPFM(s); break; }
  case ImageExtension::PIC: { IStreamStream s(stream_); img = hdrv::Image::loadPIC(s); break; }
  }
  if (!img)
  {
    std::string err = "hdrv.thumbnail error: " + img.error();
    OutputDebugStringA(err.c_str());
    return E_FAIL;
  }
  auto pic = std::move(img).value();

  // Downscale image to desired maximum resolution cx
  int maxIterations = 16;
  while ((uint32_t)pic.width() > cx || (uint32_t)pic.height() > cx)
  {
    if (--maxIterations == 0)
    {
      OutputDebugStringA("hdrv.thumbnail error: too many iterations");
      return E_FAIL;
    }

    pic = pic.scaleByHalf().value();
  }

  // Put everything into a bitmap
  uint32_t nWidth = pic.width(), nHeight = pic.height();

  BITMAPINFO bmi = { sizeof(bmi.bmiHeader) };
  bmi.bmiHeader.biWidth = nWidth;
  bmi.bmiHeader.biHeight = -static_cast<LONG>(nHeight);
  bmi.bmiHeader.biPlanes = 1;
  bmi.bmiHeader.biBitCount = 32;
  bmi.bmiHeader.biCompression = BI_RGB;

  uint8_t *pBits;
  HBITMAP hbmp = CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, reinterpret_cast<void **>(&pBits), NULL, 0);
  if (hbmp)
  {
    hr = S_OK;
    *phbmp = hbmp;
    *pdwAlpha = (pic.channels() > 3) ? WTSAT_ARGB : WTSAT_RGB;

    // Simple gamma correction, clamping and 8 bit conversion
    // (will break on negative input, e.g. normal maps)
    auto gamma = [](float in)->uint8_t {
      return (uint8_t)std::min(powf(in, 1.f / 2.2f) * 255.f, 255.f);
    };

    for (uint32_t x = 0; x < nWidth; ++x)
      for (uint32_t y = 0; y < nHeight; ++y)
      {
        const int pixelstride = 4;
        const int linestride = pixelstride * nWidth;

        uint8_t r = 0, g = 0, b = 0, a = 255;

        r = g = b = gamma(pic.value(x, y, 0));
        if (pic.channels() > 1) g = gamma(pic.value(x, y, 1));
        if (pic.channels() > 2) b = gamma(pic.value(x, y, 2));
        if (pic.channels() > 3) a = gamma(pic.value(x, y, 3));

        pBits[linestride * y + pixelstride * x + 0] = b;
        pBits[linestride * y + pixelstride * x + 1] = g;
        pBits[linestride * y + pixelstride * x + 2] = r;
        pBits[linestride * y + pixelstride * x + 3] = a;
      }
  }
  return hr;
}
