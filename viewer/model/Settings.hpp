#pragma once

#include <QObject>

namespace hdrv {

class Settings : public QObject
{
  Q_OBJECT
  Q_PROPERTY(bool thumbnailsAvailable READ thumbnailsAvailable CONSTANT FINAL)

public:
  Settings(QObject * parent = nullptr);

  bool thumbnailsAvailable() const { return thumbnailsAvailable_; }

  Q_INVOKABLE void install();
  Q_INVOKABLE void uninstall();

private:
  bool thumbnailsAvailable_ = false;
  QString thumbnailDllPath_;
};

}
