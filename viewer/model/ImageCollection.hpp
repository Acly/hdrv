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
  Q_PROPERTY(QQmlListProperty<hdrv::ImageDocument> items READ items NOTIFY itemsChanged)
  Q_PROPERTY(QList<int> recentItems READ recentItems)
  Q_PROPERTY(ImageDocument* current READ current NOTIFY currentChanged)
  Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)

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
  Q_INVOKABLE void load(QUrl const& url);
  Q_INVOKABLE void remove(int index);
  Q_INVOKABLE void replace(int index, QUrl const& url);
  Q_INVOKABLE void compare(int index);

  Q_INVOKABLE QUrl nextFile(bool prev);
  Q_INVOKABLE QStringList supportedFormats();

  ImageDocument * current() const { return items_.at(currentIndex_); }
  int currentIndex() const { return currentIndex_; }
  QList<int> const& recentItems() const { return recentItems_; };

  void setCurrentIndex(int i);

  Collection const& vector() const { return items_; }

signals:
  void itemsChanged();
  void currentChanged();
  void currentIndexChanged();
 
private slots:
  void updateRecentItems();

private:
  Collection items_;
  QList<int> recentItems_;
  int currentIndex_;
};

}

Q_DECLARE_METATYPE(QQmlListProperty<hdrv::ImageDocument>)
Q_DECLARE_METATYPE(hdrv::ImageCollection*)
