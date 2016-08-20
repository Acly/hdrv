#pragma once

#include <QObject>
#include <QUrl>

namespace hdrv {
  
class IPCClient : public QObject
{
  Q_OBJECT

public:
  Q_INVOKABLE bool remoteOpenFile(QUrl const & url);
  Q_INVOKABLE bool remoteStopServer();
  Q_INVOKABLE bool isServerAvailable();
};

}
