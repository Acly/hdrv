#pragma once

#include <QObject>

namespace hdrv {

class Settings : public QObject
{
  Q_OBJECT
  Q_PROPERTY(bool thumbnailsAvailable READ thumbnailsAvailable CONSTANT FINAL)
  Q_PROPERTY(bool singleInstance READ singleInstance WRITE setSingleInstance NOTIFY singleInstanceChanged)

public:
  Settings(QObject * parent = nullptr);

  bool thumbnailsAvailable() const { return thumbnailsAvailable_; }
  bool singleInstance() const;
  void setSingleInstance(bool singleInstance);

  Q_INVOKABLE void install();
  Q_INVOKABLE void uninstall();

signals:
  void singleInstanceChanged(bool singleInstance);

private:
  bool thumbnailsAvailable_ = false;
  QString thumbnailDllPath_;
};

}
