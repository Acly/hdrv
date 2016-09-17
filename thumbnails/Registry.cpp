#include "Registry.hpp"

const std::string ThumbnailProviderRegistration::thumbnailProviderCLSID_ = "{e357fccd-a995-4576-b01f-234630154e96}";

Registry::Registry(std::string const & subKey, OpenMode openMode, HKEY root)
{
  if (openMode == OpenMode::Create)
  {
    check(
      RegCreateKeyExA(
        root, subKey.size() ? subKey.c_str() : nullptr, 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE | DELETE,
        nullptr, &hKey_, nullptr),
      "Error creating key: " + subKey);
  }
  else
  {
    REGSAM mode;

    switch (openMode)
    {
    case OpenMode::Read: mode = KEY_READ; break;
    case OpenMode::ReadWrite: mode = KEY_READ | KEY_WRITE | DELETE; break;
    default: throw std::runtime_error("Unknown OpenMode.");
    }

    check(
      RegOpenKeyExA(root, subKey.size() ? subKey.c_str() : nullptr, 0, mode, &hKey_),
      "Error opening key: " + subKey);
  }
}

Registry::~Registry()
{
  check(RegCloseKey(hKey_), "Error closing key");
}

bool Registry::keyExists(std::string const &subKey, HKEY root)
{
  try {
    Registry(subKey, Registry::OpenMode::Read, root);
  } catch(std::runtime_error const&) {
    return false;
  }
  return true;
}

std::string Registry::readString(std::string const & valueName)
{ 
  DWORD buflen = MAX_PATH-1;
  DWORD type;
  uint8_t buf[MAX_PATH] = {};
  LSTATUS result = RegQueryValueExA(hKey_, valueName.c_str(), nullptr,
    &type, buf, &buflen);
  
  if (valueName.size() == 0 && (result == ERROR_FILE_NOT_FOUND)) {
    return "";
  }

  check(result,
    "Error reading value");
  if (type != REG_SZ && type != REG_MULTI_SZ && type != REG_EXPAND_SZ) {
    throw std::runtime_error("Value has unexpected type.");
  }

  return std::string(reinterpret_cast<char*>(buf));
}

void Registry::writeString(std::string const & valueName, std::string const & value)
{
  check(
    RegSetValueExA(hKey_, valueName.c_str(), 0,
      REG_SZ, reinterpret_cast<uint8_t const*>(value.c_str()), static_cast<DWORD>(value.size() + 1)),
    "Error writing value");
}

void Registry::deleteKeyRecursive(std::string const & subKey)
{
  check(
    RegDeleteTreeA(hKey_, subKey.c_str()),
    "Error deleting key tree");
}

void Registry::check(LSTATUS ls, std::string const & failureMsg) {
  HRESULT hr = HRESULT_FROM_WIN32(ls);
  if (FAILED(hr))
  {
    std::ostringstream ss;
    ss << "hdrv.thumbnail registry failue: " << failureMsg << "; HRESULT(0x" << std::hex << hr << ")";
    throw std::runtime_error(ss.str());
  }
}


void ThumbnailProviderRegistration::registerInprocServer(std::string const & inprocSvrClsid, std::string const & friendlyName, std::string const & absoluteSvrPath)
{
  const std::string keyBase = "CLSID\\" + inprocSvrClsid;
  Registry(keyBase, Registry::OpenMode::Create).writeString("", friendlyName);
  Registry inprocSvr(keyBase + "\\InprocServer32", Registry::OpenMode::Create);
  inprocSvr.writeString("", absoluteSvrPath);
  inprocSvr.writeString("ThreadingModel", "Apartment");
}

void ThumbnailProviderRegistration::unregisterInprocServer(std::string const & inprocSvrClsid)
{
  Registry("CLSID").deleteKeyRecursive(inprocSvrClsid);
}

void ThumbnailProviderRegistration::registerThumbnailHandler(std::string const & fileExtension, std::string const & inprocSvrClsid)
{
  std::string fileType = Registry(fileExtension, Registry::OpenMode::Create).readString();
  std::string keyBase = fileExtension;
  if (fileType.size() > 0) {
    keyBase = fileType;
  }
  Registry(keyBase + "\\shellex\\" + thumbnailProviderCLSID_, Registry::OpenMode::Create)
    .writeString("", inprocSvrClsid);
}

void ThumbnailProviderRegistration::unregisterThumbnailHandler(std::string const & fileExtension)
{
  std::string fileType = Registry(fileExtension).readString();
  if (fileType.size() > 0) {
    removeKeyIfExists(fileExtension + "\\shellex\\" + thumbnailProviderCLSID_);
  }
  removeKeyIfExists(fileType + "\\shellex\\" + thumbnailProviderCLSID_);
}

void ThumbnailProviderRegistration::removeKeyIfExists(std::string const & key) {
  if (Registry::keyExists(key)) {
    Registry().deleteKeyRecursive(key);
  }
}
