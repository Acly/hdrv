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

bool ImageCollection::add(QUrl const& url, Result<Image> && image)
{
  if (image) {
    add(new ImageDocument(std::make_shared<Image>(std::move(image).value()), url, this));
  } else {
    setError(QString::fromStdString(image.error()) + " while loading file " + url.toLocalFile());
  }
  return (bool)image;
}

bool ImageCollection::load(QUrl const& url)
{
  QFileInfo file(url.toLocalFile());
  if (!file.exists()) {
    setError("File " + file.absoluteFilePath() + " does not exist.");
    return false;
  }
  if (file.suffix() == "hdr" || file.suffix() == "pic") {
    return add(url, Image::loadPIC(file.absoluteFilePath().toStdString()));
  } else if (file.suffix() == "pfm" || file.suffix() == "ppm") {
    return add(url, Image::loadPFM(file.absoluteFilePath().toStdString()));
  }
  setError("Unknown file extension: " + file.suffix());
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

void ImageCollection::setError(QString const& message)
{
  errorMessage_ = message;
  emit errorMessageChanged();
}

}