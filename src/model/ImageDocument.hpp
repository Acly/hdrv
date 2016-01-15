#pragma once

#include <memory>

#include <QObject>
#include <QSize>
#include <QPoint>
#include <QUrl>

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
  Q_PROPERTY(float scale READ scale NOTIFY scaleChanged)
  Q_PROPERTY(int scaleIndex READ scaleIndex WRITE setScaleIndex NOTIFY scaleIndexChanged)
  Q_PROPERTY(int maxScaleIndex READ maxScaleIndex CONSTANT FINAL)
  Q_PROPERTY(qreal gamma READ gamma WRITE setGamma NOTIFY gammaChanged)
  Q_PROPERTY(qreal minGamma READ minGamma CONSTANT FINAL)
  Q_PROPERTY(qreal maxGamma READ maxGamma CONSTANT FINAL)

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
  int scaleIndex() const { return scaleIndex_; }
  int maxScaleIndex() const { return 4; }
  qreal gamma() const { return gamma_; }
  qreal minGamma() const { return 1.0; }
  qreal maxGamma() const { return 8.0; }
  float const* pixels() const { return image_->data(); }
  std::shared_ptr<Image> const& image() { return image_; }
  bool isDefault() const;

  void setPosition(QPointF pos);
  void move(QPointF offset);
  void setScaleIndex(int index);
  void setGamma(qreal gamma);

signals:
  void propertyChanged();
  void positionChanged();
  void scaleChanged();
  void scaleIndexChanged();
  void gammaChanged();
  
private:
  QString name_;
  QUrl url_;
  QPointF position_;
  int scaleIndex_;
  qreal gamma_;
  std::shared_ptr<Image> image_;
};

Q_DECLARE_METATYPE(ImageDocument*);

}