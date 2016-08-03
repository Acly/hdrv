#include <QGuiApplication>
#include <QQmlComponent>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlProperty>
#include <QDebug>

#include <model/ImageDocument.hpp>
#include <model/ImageCollection.hpp>
#include <model/Settings.hpp>
#include <view/ImageArea.hpp>

using namespace hdrv;

int main(int argc, char * argv[])
{
  QGuiApplication app(argc, argv);
  qmlRegisterType<ImageDocument>("Hdrv", 1, 0, "ImageDocument");
  qmlRegisterType<ImageCollection>("Hdrv", 1, 0, "ImageCollection");
  qmlRegisterType<ImageArea>("Hdrv", 1, 0, "ImageArea");
  qmlRegisterType<Settings>("Hdrv", 1, 0, "Settings");

  Settings settings;
  ImageCollection images;
  QQmlApplicationEngine engine;
  engine.rootContext()->setContextProperty("settings", &settings);
  engine.rootContext()->setContextProperty("images", &images);

  for (int i = 1; i < app.arguments().count(); ++i) {
    images.load(QUrl::fromLocalFile(app.arguments()[i]));
  }

  engine.load(QUrl("qrc:/hdrv/view/Main.qml"));

  return app.exec();
}
