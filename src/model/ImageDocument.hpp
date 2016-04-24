#pragma once

#include <memory>

#include <QObject>
#include <QSize>
#include <QPoint>
#include <QUrl>
#include <QVector4D>

#include <image/Image.hpp>

namespace hdrv {

class ImageDocument : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString name READ name CONSTANT)
  Q_PROPERTY(QUrl url READ url CONSTANT)
  Q_PROPERTY(QSize size READ size CONSTANT)
  Q_PROPERTY(int channels READ channels CONSTANT)
  Q_PROPERTY(QPointF position READ position WRITE setPosition NOTIFY positionChanged)
  Q_PROPERTY(float scale READ scale WRITE setScale NOTIFY scaleChanged)
  Q_PROPERTY(qreal brightness READ brightness WRITE setBrightness NOTIFY brightnessChanged)
  Q_PROPERTY(qreal minBrightness READ minBrightness CONSTANT FINAL)
  Q_PROPERTY(qreal maxBrightness READ maxBrightness CONSTANT FINAL)
  Q_PROPERTY(qreal gamma READ gamma WRITE setGamma NOTIFY gammaChanged)
  Q_PROPERTY(qreal minGamma READ minGamma CONSTANT FINAL)
  Q_PROPERTY(qreal maxGamma READ maxGamma CONSTANT FINAL)
  Q_PROPERTY(bool isFloat READ isFloat CONSTANT FINAL)
  Q_PROPERTY(QPoint pixelPosition READ pixelPosition NOTIFY pixelPositionChanged)
  Q_PROPERTY(QVector4D pixelValue READ pixelValue NOTIFY pixelValueChanged)

public:
  ImageDocument(std::shared_ptr<Image> image, QUrl const& url, QObject * parent = nullptr);
  ImageDocument(QObject * parent = nullptr);

  QString const& name() const { return name_; }
  QUrl const& url() const { return url_; }
  QPointF position() const { return position_; }
  int width() const { return image_->width();  }
  int height() const { return image_->height(); }
  QSize size() const { return QSize(image_->width(), image_->height()); }
  int channels() const { return image_->channels(); }
  float scale() const;
  qreal brightness() const { return brightness_; }
  qreal minBrightness() const { return -10.0; }
  qreal maxBrightness() const { return 10.0; }
  qreal gamma() const { return image_->format() == Image::Float ? gamma_ : 2.2; }
  qreal minGamma() const { return 1.0; }
  qreal maxGamma() const { return 8.0; }
  bool isFloat() const { return image_->format() == Image::Float; }
  void const* pixels() const { return image_->data(); }
  std::shared_ptr<Image> const& image() { return image_; }
  QPoint pixelPosition() const { return pixelPosition_; }
  QVector4D pixelValue() const;
  bool isDefault() const;

  void setPosition(QPointF pos);
  void move(QPointF offset);
  void setScale(qreal scale);
  void setBrightness(qreal brightness);
  void setGamma(qreal gamma);
  void setCurrentPixel(QPoint index);

signals:
  void propertyChanged();
  void positionChanged();
  void scaleChanged();
  void brightnessChanged();
  void gammaChanged();
  void pixelPositionChanged();
  void pixelValueChanged();
  
private:
  QString name_;
  QUrl url_;
  QPointF position_;
  qreal scale_;
  qreal brightness_;
  qreal gamma_;
  QPoint pixelPosition_;
  QVector4D pixelValue_;
  std::shared_ptr<Image> image_;
};

Q_DECLARE_METATYPE(ImageDocument*);

}