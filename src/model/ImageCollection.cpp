#include <model/ImageCollection.hpp>

#include <QFileInfo>

namespace hdrv {

ImageCollection::ImageCollection(QObject * parent)
  : QObject(parent)
  , currentIndex_(-1)
{
  add(new ImageDocument);
  setCurrentIndex(0);
}
  
void ImageCollection::add(ImageDocument * image)
{
  if (items_.size() == 1 && items_[0]->isDefault()) {
    items_.clear();
  }
  items_.emplace_back(image);
  currentIndex_ = (int)items_.size() - 1;
  emit itemsChanged();
  emit currentIndexChanged();
  emit currentChanged();
}

void ImageCollection::load(QUrl const& url)
{
  add(new ImageDocument(url, this));
}

void ImageCollection::remove(int index)
{
  auto item = items_[index];
  currentIndex_ = std::min(currentIndex(), (int)items_.size() - 2);
  items_.erase(items_.begin() + index);
  emit itemsChanged();
  emit currentIndexChanged();
  emit currentChanged();
  item->deleteLater();
}

void ImageCollection::compare(int index)
{
  add(new ImageDocument(current()->url(), items_[index]->url(), this));
}

void ImageCollection::setCurrentIndex(int i)
{
  if (currentIndex_ != i) {
    currentIndex_ = i;
    emit currentIndexChanged();
    emit currentChanged();
  }
}

}