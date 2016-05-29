#pragma once

#include <QLocalServer>

namespace hdrv {

class IPCServer : public QObject
{
  Q_OBJECT
  Q_PROPERTY(bool running READ isRunning NOTIFY runningChanged)

public:
  IPCServer(QObject * parent = nullptr);

  Q_INVOKABLE void start();
  Q_INVOKABLE void stop();

  bool isRunning();

signals:
  void runningChanged();
  void openFile(QUrl const & url);

private slots:
  void newConnection();

private:
  QLocalServer* localServer_;
};

}
