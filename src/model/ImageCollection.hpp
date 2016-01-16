#pragma once

#include <QDebug>
#include <QObject>
#include <QList>
#include <QQmlListProperty>
#include <QUrl>

#include <model/ImageDocument.hpp>

namespace hdrv {

class ImageCollection : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QQmlListProperty<ImageDocument> items READ items NOTIFY itemsChanged)
  Q_PROPERTY(ImageDocument* current READ current NOTIFY currentChanged)
  Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
  Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)

  using Collection = std::vector<ImageDocument *>;

public:
  ImageCollection(QObject * parent = nullptr);

  static int itemCount(QQmlListProperty<ImageDocument> * list)
  {
    return (int)reinterpret_cast<Collection *>(list->data)->size();
  }

  static ImageDocument * itemAt(QQmlListProperty<ImageDocument> * list, int index)
  {
    return reinterpret_cast<Collection *>(list->data)->at(index);
  }

  QQmlListProperty<ImageDocument> items()
  {
    return QQmlListProperty<ImageDocument>(this, &items_, &itemCount, &itemAt);
  }

  void add(ImageDocument * image);
  bool add(QUrl const& url, Result<Image> && image);
  Q_INVOKABLE bool load(QUrl const& url);
  Q_INVOKABLE void remove(int index);

  ImageDocument * current() const { return items_.at(currentIndex_); }
  int currentIndex() const { return currentIndex_; }
  QString const& errorMessage() const { return errorMessage_; }
  
  void setCurrentIndex(int i);
  void setError(QString const& message);

  Collection const& vector() const { return items_; }

signals:
  void itemsChanged();
  void currentChanged();
  void currentIndexChanged();
  void errorMessageChanged();

private:
  Collection items_;
  int currentIndex_;
  QString errorMessage_;
};

Q_DECLARE_METATYPE(QQmlListProperty<ImageDocument>)
Q_DECLARE_METATYPE(ImageCollection*)

}