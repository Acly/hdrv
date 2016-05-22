#include <model/ImageCollection.hpp>

#include <QFileInfo>
#include <QDir>

namespace hdrv {

ImageCollection::ImageCollection(QObject * parent)
  : QObject(parent)
  , currentIndex_(-1)
{
  connect(this, SIGNAL(currentIndexChanged()), this, SLOT(updateRecentItems()));

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

void ImageCollection::replace(int index, QUrl const& url)
{
  auto item = items_[index];
  item->deleteLater();
  items_[index] = new ImageDocument(url, this);
  items_[index]->setPosition(item->position());
  items_[index]->setScale(item->scale());
  emit itemsChanged();
  emit currentIndexChanged();
  emit currentChanged();
}

void ImageCollection::remove(int index)
{
  auto item = items_[index];
  recentItems_.removeOne(index);
  currentIndex_ = std::min(currentIndex(), (int)items_.size() - 2);
  items_.erase(items_.begin() + index);
  emit itemsChanged();
  emit currentIndexChanged();
  emit currentChanged();
  item->deleteLater();
}

QUrl ImageCollection::nextFile(bool prev)
{
  if (items_.size() == 1 && items_[0]->isDefault()) {
    return QUrl();
  }
  
  QFileInfo currentFile(items_[currentIndex()]->url().toLocalFile());
  QDir dir(currentFile.absolutePath());
  
  auto list = dir.entryList(supportedFormats(), QDir::Files, QDir::Name | QDir::IgnoreCase);
  int index = list.indexOf(currentFile.fileName());
  if (index == -1) {
    return QUrl();
  }

  index += (prev ? -1 : 1);
  index = index < 0 ? list.size() - 1 : index;
  index = index >= list.size() ? 0 : index;
  
  QFileInfo nextFile(dir, list[index]);
  return QUrl::fromLocalFile(nextFile.absoluteFilePath());
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

void ImageCollection::updateRecentItems()
{
  int index = currentIndex();
  recentItems_.removeOne(index);
  recentItems_.push_back(index);
}

QStringList ImageCollection::supportedFormats()
{
  return QStringList()
    << "*.png"
    << "*.jpg"
    << "*.bmp"
    << "*.gif"
    << "*.hdr"
    << "*.pic"
    << "*.pfm"
    << "*.ppm"
    << "*.exr";
}

}