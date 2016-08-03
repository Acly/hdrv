#pragma once

#include "ThumbnailProvider.hpp"

#include <unknwn.h> // IClassFactory
#include <windows.h>
#include <atomic>

class ClassFactory : public IClassFactory
{
public:
  // IUnknown
  IFACEMETHODIMP QueryInterface(REFIID riid, void **ppv);
  IFACEMETHODIMP_(ULONG) AddRef();
  IFACEMETHODIMP_(ULONG) Release();

  // IClassFactory
  IFACEMETHODIMP CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppv);
  IFACEMETHODIMP LockServer(BOOL fLock);

  ClassFactory(ThumbnailProvider::ImageExtension ext);

protected:
  ~ClassFactory();

private:
  std::atomic<long> referenceCounter_;
  ThumbnailProvider::ImageExtension imageExtension_;
};
