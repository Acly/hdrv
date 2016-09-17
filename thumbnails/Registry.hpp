#pragma once

#include <string>
#include <cstdint>
#include <sstream>

#include <windows.h>

// Why Microsoft, why?
#ifdef min
# undef min
#endif

class Registry
{
public:
  enum class OpenMode { Create, ReadWrite, Read };

  Registry(std::string const &subKey = "", OpenMode openMode = OpenMode::ReadWrite, HKEY root = HKEY_CLASSES_ROOT);
  ~Registry();

  static bool keyExists(std::string const &subKey, HKEY root = HKEY_CLASSES_ROOT);

  std::string readString(std::string const &valueName = "");
  void writeString(std::string const &valueName, std::string const &value);
  void deleteKeyRecursive(std::string const &subKey);

protected:
  void check(LSTATUS ls, std::string const &failureMsg);

  HKEY hKey_ = 0;
};

class ThumbnailProviderRegistration
{
public:
  static void registerInprocServer(std::string const &inprocSvrClsid, std::string const &friendlyName, std::string const &absoluteSvrPath);
  static void unregisterInprocServer(std::string const &inprocSvrClsid);

  static void registerThumbnailHandler(std::string const &fileExtension, std::string const &inprocSvrClsid);
  static void unregisterThumbnailHandler(std::string const &fileExtension);

protected:
  static void removeKeyIfExists(std::string const &key);

  static const std::string thumbnailProviderCLSID_;
};
