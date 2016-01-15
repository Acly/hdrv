#include <model/ImageCollection.hpp>

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

bool ImageCollection::load(QUrl const& url)
{
  setLastLoadedFile(url.toLocalFile());
  auto image = Image::loadPIC(url.toLocalFile().toStdString());
  if (image) {
    add(new ImageDocument(std::make_shared<Image>(std::move(image).value()), url, this));
    return true;
  }
  qDebug() << QString::fromStdString(image.error());
  return false;
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

void ImageCollection::setCurrentIndex(int i)
{
  if (currentIndex_ != i) {
    currentIndex_ = i;
    emit currentIndexChanged();
    emit currentChanged();
  }
}

void ImageCollection::setLastLoadedFile(QString const& filename)
{
  lastLoadedFile_ = filename;
  emit lastLoadedFileChanged();
}

}