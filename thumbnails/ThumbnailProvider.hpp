#pragma once

#include <windows.h>
#include <thumbcache.h> // IThumbnailProvider

#include <atomic>

class ThumbnailProvider :
  public IInitializeWithStream,
  public IThumbnailProvider
{
public:

  enum class ImageExtension { PFM, PIC, EXR };

  // IUnknown
  IFACEMETHODIMP QueryInterface(REFIID riid, void **ppv);
  IFACEMETHODIMP_(ULONG) AddRef();
  IFACEMETHODIMP_(ULONG) Release();

  // IInitializeWithStream
  IFACEMETHODIMP Initialize(IStream *stream, DWORD grfMode);

  // IThumbnailProvider
  IFACEMETHODIMP GetThumbnail(UINT cx, HBITMAP *phbmp, WTS_ALPHATYPE *pdwAlpha);

  ThumbnailProvider(ImageExtension ext);

protected:
  ~ThumbnailProvider();

private:
  std::atomic<long> referenceCounter_;
  IStream *stream_;
  ImageExtension imageExtension_;
};
