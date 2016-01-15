#include <model/ImageDocument.hpp>

namespace hdrv {

float const scaleValues[] = { 0.25f, 0.5f, 1.0f, 2.0f, 4.0f };
  
std::shared_ptr<Image> createDefaultImage()
{
  return std::make_shared<Image>(0, 0, 3, std::unique_ptr<float[]>(new float[1]));
}

QUrl defaultUrl() { return QUrl("file:////HDRV"); }

ImageDocument::ImageDocument(std::shared_ptr<Image> image, QUrl const& url, QObject * parent)
  : QObject(parent)
  , name_(url.fileName())
  , url_(url)
  , scaleIndex_(2)
  , gamma_(2.2f)
  , image_(std::move(image))
{}

ImageDocument::ImageDocument(QObject * parent) 
  : ImageDocument(createDefaultImage(), defaultUrl(), parent) 
{}

float ImageDocument::scale() const { return scaleValues[scaleIndex_]; }
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

void ImageDocument::setScaleIndex(int index)
{
  if (scaleIndex_ != index && index >= 0 && index <= maxScaleIndex()) {
    scaleIndex_ = index;
    emit scaleIndexChanged();
    emit scaleChanged();
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

}
