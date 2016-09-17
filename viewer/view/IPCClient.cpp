#include "IPCClient.hpp"

#include <QLocalSocket>
#include <QFileInfo>

namespace hdrv {

namespace {

bool sendCommand(QString const & command) {
  QLocalSocket socket;
  socket.connectToServer("hdrv");
  if (socket.waitForConnected(50)) {
    socket.write(command.toUtf8());
    if (socket.waitForBytesWritten(500)) {
      socket.close();
      socket.waitForDisconnected(50);
      return true;
    }
  }

  return false;
}

}

bool IPCClient::remoteOpenFile(QUrl const & url)
{
  QFileInfo f(url.toLocalFile());
  return sendCommand(QString("open %1").arg(f.absoluteFilePath()));
}

bool IPCClient::remoteStopServer()
{
  return sendCommand("exit");
}

bool IPCClient::isServerAvailable()
{
  return sendCommand("");
}

}
