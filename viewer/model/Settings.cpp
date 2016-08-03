#include <model/Settings.hpp>

#include <sstream>

#include <QDir>
#include <QFileInfo>
#include <QCoreApplication>

#ifdef WIN32
# include <windows.h>
#endif // WIN32

namespace hdrv {

Settings::Settings(QObject * parent)
  : QObject(parent)
{
#ifdef WIN32
  QFileInfo thumbDll(QDir(QCoreApplication::applicationDirPath()), "thumbnails.dll");
  thumbnailsAvailable_ = thumbDll.exists();
  if (thumbnailsAvailable_) {
    thumbnailDllPath_ = thumbDll.absoluteFilePath();
  }
#endif // WIN32
}

void registerDll(std::string const& dllPath, bool remove = false)
{
#ifdef WIN32
  std::ostringstream oss;
  oss << "/s";
  if (remove) {
    oss << " /u";
  }
  oss << " \"" << dllPath << "\"";
  ShellExecuteA(0, "runas", "regsvr32.exe", oss.str().c_str(), nullptr, SW_SHOWNORMAL);
#endif // WIN32
}

void Settings::install()
{
  registerDll(thumbnailDllPath_.toStdString());
}

void Settings::uninstall()
{
  registerDll(thumbnailDllPath_.toStdString(), true);
}

}
