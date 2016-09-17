#include "IPCServer.hpp"

#include <QLocalSocket>
#include <QUrl>

namespace hdrv {

IPCServer::IPCServer(QObject * parent)
  : QObject(parent)
  , localServer_(nullptr)
{
  localServer_ = new QLocalServer(this);
  
  connect(localServer_, SIGNAL(newConnection()), this, SLOT(newConnection()));
}

void IPCServer::start()
{
  if (!localServer_->isListening()) {
    localServer_->listen("hdrv");
    emit runningChanged();
  }
}

void IPCServer::stop()
{
  if (localServer_->isListening()) {
    localServer_->close();
    emit runningChanged();
  }
}

bool IPCServer::isRunning()
{
  return localServer_->isListening();
}

void IPCServer::newConnection()
{
  while (localServer_->hasPendingConnections()) {
    auto * socket = localServer_->nextPendingConnection();
    
    connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
    connect(socket, &QLocalSocket::readyRead, [&, socket]() {
      while (socket->bytesAvailable() > 0) {
        QString cmd = socket->readLine();
        if (cmd.startsWith("open")) {
          QString filename = cmd.right(cmd.length() - 5);
          emit openFile(QUrl::fromLocalFile(filename));
        } else if (cmd.startsWith("exit")) {
          socket->disconnectFromServer();
          stop();
        }
      }
    });
  }
}

}
