#include "Thumbnails.hpp"

#include <Guiddef.h>
#include <shlobj.h>

#include "Registry.hpp"
#include "ClassFactory.hpp"
#include "ThumbnailProvider.hpp"

#include <vector>
#include <locale>
#include <codecvt>
#include <algorithm>

#pragma comment( linker, "/export:DllGetClassObject,PRIVATE" )
#pragma comment( linker, "/export:DllCanUnloadNow,PRIVATE" )
#pragma comment( linker, "/export:DllRegisterServer,PRIVATE" )
#pragma comment( linker, "/export:DllUnregisterServer,PRIVATE" )

HINSTANCE ThumbnailsDll::hInstance = NULL;
std::atomic<long> ThumbnailsDll::dllReferenceCounter = 0;

struct ExtensionProvider
{
    CLSID clsid;
    ThumbnailProvider::ImageExtension extensionType;
    std::vector<std::string> extensionList;
};

const std::vector<ExtensionProvider> supportedExtensions{
  // {20EEDC07-F4E2-428C-9D35-F48EBF5B4D00}
  { { 0x20eedc07, 0xf4e2, 0x428c, { 0x9d, 0x35, 0xf4, 0x8e, 0xbf, 0x5b, 0x4d, 0x00 } }, ThumbnailProvider::ImageExtension::PIC, { ".hdr", ".pic" } },
  // {20EEDC07-F4E2-428C-9D35-F48EBF5B4D01}
  { { 0x20eedc07, 0xf4e2, 0x428c, { 0x9d, 0x35, 0xf4, 0x8e, 0xbf, 0x5b, 0x4d, 0x01 } }, ThumbnailProvider::ImageExtension::PFM, { ".pfm", ".ppm" } },
  // {20EEDC07-F4E2-428C-9D35-F48EBF5B4D02}
  { { 0x20eedc07, 0xf4e2, 0x428c, { 0x9d, 0x35, 0xf4, 0x8e, 0xbf, 0x5b, 0x4d, 0x02 } }, ThumbnailProvider::ImageExtension::EXR, { ".exr" } }
};

std::string clsidToString(CLSID const& clsid)
{
  wchar_t wclsid[MAX_PATH];
  StringFromGUID2(clsid, wclsid, ARRAYSIZE(wclsid));
  return std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(wclsid);
}

std::string getModuleFilePath()
{
  char moduleName[MAX_PATH];
  if (GetModuleFileNameA(ThumbnailsDll::hInstance, moduleName, MAX_PATH) == 0)
  {
    std::ostringstream ss;
    ss << "hdrv.thumbnail failure: could not retrieve module file name; HRESULT(0x" << std::hex << HRESULT_FROM_WIN32(GetLastError()) << ")";
    throw std::runtime_error(ss.str());
  }
  return moduleName;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID /*lpReserved*/)
{
  if(dwReason == DLL_PROCESS_ATTACH) {
    ThumbnailsDll::hInstance = hModule;
    DisableThreadLibraryCalls(hModule);
  }
  return TRUE;
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void **ppv)
{
  HRESULT hr = CLASS_E_CLASSNOTAVAILABLE;

  auto &extProviderIt = std::find_if(supportedExtensions.cbegin(), supportedExtensions.cend(), [&rclsid](ExtensionProvider const &provider)->bool {
    return IsEqualCLSID(provider.clsid, rclsid) == TRUE;
  });

  if (extProviderIt != supportedExtensions.cend())
  {
    hr = E_OUTOFMEMORY;

    ClassFactory *pClassFactory = new (std::nothrow) ClassFactory(extProviderIt->extensionType);
    if (pClassFactory)
    {
      hr = pClassFactory->QueryInterface(riid, ppv);
      pClassFactory->Release();
    }
  }

  return hr;
}

STDAPI DllCanUnloadNow(void)
{
  return ThumbnailsDll::dllReferenceCounter > 0 ? S_FALSE : S_OK;
}

STDAPI DllRegisterServer(void)
{
  HRESULT hr = S_OK;

  try
  {
    std::string moduleName = getModuleFilePath();

    for (auto const & ext : supportedExtensions)
    {
      std::string clsid = clsidToString(ext.clsid);

      // Register the component.
      ThumbnailProviderRegistration::registerInprocServer(clsid, "hdrv.ThumbnailProvider Class", moduleName);

      // Register the thumbnail handler.
      for (auto const &extension : ext.extensionList) {
        ThumbnailProviderRegistration::registerThumbnailHandler(extension, clsid);
      }

      // This tells the shell to invalidate the thumbnail cache. It is 
      // important because any of our files viewed before registering 
      // this handler would otherwise show cached blank thumbnails.
      SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
    }
  }
  catch (std::exception const &e)
  {
    std::ostringstream ss;
    ss << "hdrv.thumbnail - Failed to register thumbnail handler:" << std::endl << std::endl << e.what();
    std::string err = ss.str();
    MessageBoxA(0, err.c_str(), "hdrv.thumbnail - Failed to register thumbnail handler", MB_ICONERROR | MB_OK);
    hr = E_FAIL;
  }

  return hr;
}

STDAPI DllUnregisterServer(void)
{
  HRESULT hr = S_OK;

  for (auto const & ext : supportedExtensions)
  {
    try
    {
      // Unregister the component.
      ThumbnailProviderRegistration::unregisterInprocServer(clsidToString(ext.clsid));

      // Unregister the thumbnail handler.
      for (auto const &extension : ext.extensionList) {
        ThumbnailProviderRegistration::unregisterThumbnailHandler(extension);
      }
    }
    catch (std::exception const &e)
    {
      // only show one error message
      if (SUCCEEDED(hr))
      {
        std::ostringstream ss;
        ss << "hdrv.thumbnail - Failed to unregister thumbnail handler - was it already unregistered?" << std::endl << std::endl << e.what();
        std::string err = ss.str();
        MessageBoxA(0, err.c_str(), "hdrv.thumbnail - Failed to register thumbnail handler", MB_ICONERROR | MB_OK);
        hr = E_FAIL;
      }
    }
  }

  return hr;
}
