#include <QGuiApplication>
#include <QQmlComponent>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlProperty>

#include <model/ImageDocument.hpp>
#include <model/ImageCollection.hpp>
#include <model/Settings.hpp>
#include <view/ImageArea.hpp>
#include <view/IPCServer.hpp>
#include <view/IPCClient.hpp>

using namespace hdrv;

int main(int argc, char * argv[])
{
  QGuiApplication app(argc, argv);
  qmlRegisterType<ImageDocument>("Hdrv", 1, 0, "ImageDocument");
  qmlRegisterType<ImageCollection>("Hdrv", 1, 0, "ImageCollection");
  qmlRegisterType<ImageArea>("Hdrv", 1, 0, "ImageArea");
  qmlRegisterType<Settings>("Hdrv", 1, 0, "Settings");
  qmlRegisterType<IPCServer>("Hdrv", 1, 0, "Server");
  qmlRegisterType<IPCClient>("Hdrv", 1, 0, "Client");

  Settings settings;
  IPCServer server;
  IPCClient client;
  ImageCollection images;
  QQmlApplicationEngine engine;
  engine.rootContext()->setContextProperty("settings", &settings);
  engine.rootContext()->setContextProperty("images", &images);
  engine.rootContext()->setContextProperty("server", &server);
  engine.rootContext()->setContextProperty("client", &client);

  QObject::connect(&server, SIGNAL(openFile(QUrl const &)), &images, SLOT(load(QUrl const &)));

  bool fileOpened = false;
  for (int i = 1; i < app.arguments().count(); ++i) {
    auto url = QUrl::fromLocalFile(app.arguments()[i]);
    if (!client.remoteOpenFile(url)) {
      images.load(url);
      fileOpened = true;
    }
  }

  if (app.arguments().count() > 1 && !fileOpened) {
    return 0;
  }

  if (settings.singleInstance() && !client.isServerAvailable()) {
    server.start();
  }

  engine.load(QUrl("qrc:/hdrv/view/Main.qml"));

  return app.exec();
}
