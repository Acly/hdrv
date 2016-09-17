#include "ClassFactory.hpp"
#include "Thumbnails.hpp"
#include "ThumbnailProvider.hpp"
#include <new>

#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

ClassFactory::ClassFactory(ThumbnailProvider::ImageExtension ext)
  : referenceCounter_(1), imageExtension_(ext)
{
  ++ThumbnailsDll::dllReferenceCounter;
}

ClassFactory::~ClassFactory()
{
  --ThumbnailsDll::dllReferenceCounter;
}

// IUnknown

#pragma warning(push)
#pragma warning(disable: 4838) // conversion from 'DWORD' to 'int' requires a narrowing conversion

IFACEMETHODIMP ClassFactory::QueryInterface(REFIID riid, void **ppv)
{
  static const QITAB qit[] =
  {
      QITABENT(ClassFactory, IClassFactory),
      { 0 },
  };
  return QISearch(this, qit, riid, ppv);
}

#pragma warning(pop)

IFACEMETHODIMP_(ULONG) ClassFactory::AddRef()
{
  return ++referenceCounter_;
}

IFACEMETHODIMP_(ULONG) ClassFactory::Release()
{
  ULONG cRef = --referenceCounter_;
  if (0 == cRef)
  {
    delete this;
  }
  return cRef;
}

// IClassFactory

IFACEMETHODIMP ClassFactory::CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppv)
{
  HRESULT hr = CLASS_E_NOAGGREGATION;

  // pUnkOuter is used for aggregation. We do not support it.
  if (pUnkOuter == NULL)
  {
    hr = E_OUTOFMEMORY;

    // Create the COM component.
    ThumbnailProvider *pExt = new (std::nothrow) ThumbnailProvider(imageExtension_);
    if (pExt)
    {
      // Query the specified interface.
      hr = pExt->QueryInterface(riid, ppv);
      pExt->Release();
    }
  }

  return hr;
}

IFACEMETHODIMP ClassFactory::LockServer(BOOL fLock)
{
  if (fLock)
  {
    ++ThumbnailsDll::dllReferenceCounter;
  }
  else
  {
    --ThumbnailsDll::dllReferenceCounter;
  }
  return S_OK;
}
