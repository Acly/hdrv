#include <model/ImageDocument.hpp>

#include <QDir>
#include <QFileInfo>

#include <model/ImageCollection.hpp>

namespace hdrv {

float const scaleValues[] = { 0.25f, 0.5f, 1.0f, 2.0f, 4.0f };
  
std::shared_ptr<Image> createDefaultImage()
{
  return std::make_shared<Image>(Image::makeEmpty());
}

QUrl defaultUrl() { return QUrl("file:////HDRV"); }

ImageDocument::ImageDocument(std::shared_ptr<Image> image, QUrl const& url, QObject * parent)
  : QObject(parent)
  , name_(url.fileName())
  , url_(url)
  , scale_(1.0f)
  , brightness_(0.0f)
  , gamma_(2.2f)
  , image_(std::move(image))
{}

ImageDocument::ImageDocument(QObject * parent) 
  : ImageDocument(createDefaultImage(), defaultUrl(), parent) 
{}

QUrl ImageDocument::directory() const
{
  return QUrl::fromLocalFile(QFileInfo(url().toLocalFile()).absolutePath());
}

bool ImageDocument::isDefault() const { return url() == defaultUrl(); }

void ImageDocument::setPosition(QPointF pos)
{
  position_ = pos;
  emit positionChanged();
  emit propertyChanged();
}

void ImageDocument::move(QPointF offset)
{
  setPosition(position() + offset);
}

void ImageDocument::setScale(qreal scale)
{
  if (!qFuzzyCompare(scale_, scale)) {
    scale_ = scale;
    emit scaleChanged();
    emit propertyChanged();
  }
}

void ImageDocument::setBrightness(qreal brightness)
{
  if (!qFuzzyCompare(brightness_, brightness) && brightness >= minBrightness() && brightness <= maxBrightness()) {
    brightness_ = brightness;
    emit brightnessChanged();
    emit propertyChanged();
  }
}

void ImageDocument::setGamma(qreal gamma)
{
  if (!qFuzzyCompare(gamma_, gamma) && gamma >= minGamma() && gamma <= maxGamma()) {
    gamma_ = gamma;
    emit gammaChanged();
    emit propertyChanged();
  }
}

void ImageDocument::setCurrentPixel(QPoint index)
{
  if (index != pixelPosition_ && index.x() < width() && index.y() < height()) {
    pixelPosition_ = index;
    emit pixelPositionChanged();
    emit pixelValueChanged();
  }
}

bool ImageDocument::store(QUrl const& url)
{
  QFileInfo file(url.toLocalFile());
  if (file.suffix() == "hdr" || file.suffix() == "pic") {
    return check(image()->storePIC(file.absoluteFilePath().toStdString()));
  } else if (file.suffix() == "pfm" || file.suffix() == "ppm") {
    return check(image()->storePFM(file.absoluteFilePath().toStdString()));
  }
  setError("Unsupported file extension: " + file.suffix());
  return false;
}

QVector4D ImageDocument::pixelValue() const
{
  QVector4D texel;
  texel.setX(image_->value<float>(pixelPosition_.x(), pixelPosition_.y(), 0));
  if (channels() > 1) {
    texel.setY(image_->value<float>(pixelPosition_.x(), pixelPosition_.y(), 1));
    if (channels() > 2) {
      texel.setZ(image_->value<float>(pixelPosition_.x(), pixelPosition_.y(), 2));
      if (channels() > 3) {
        texel.setW(image_->value<float>(pixelPosition_.x(), pixelPosition_.y(), 3));
      }
    }
  }
  return texel;
}

bool ImageDocument::check(Result<bool> const& result)
{
  if (!result) {
    setError(QString::fromStdString(result.error()));
  }
  return (bool)result;
}

void ImageDocument::setError(QString const& message)
{
  auto & collection = *static_cast<ImageCollection *>(parent());
  collection.setError(message);
}

}
