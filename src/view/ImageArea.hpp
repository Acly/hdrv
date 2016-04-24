#pragma once

#include <memory>

#include <QQuickItem>
#include <QQuickWindow>
#include <QHoverEvent>

#include <model/ImageCollection.hpp>
#include <view/ImageRenderer.hpp>

namespace hdrv {

class ImageArea : public QQuickItem
{
  Q_OBJECT
  Q_PROPERTY(ImageCollection* model READ model WRITE setModel NOTIFY modelChanged)
  Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

public:
  ImageArea();

  ImageCollection * model() const { return images_; }
  QColor const& color() const { return color_; }

  void setModel(ImageCollection * v);
  void setColor(QColor const& color);

signals:
  void modelChanged();
  void colorChanged();

public slots:
  void sync();
  void cleanup() { renderer_ = nullptr; }

protected:
  void mousePressEvent(QMouseEvent * event) override;
  void mouseReleaseEvent(QMouseEvent * event) override;
  void mouseMoveEvent(QMouseEvent * event) override;
  void wheelEvent(QWheelEvent * event) override;
  void hoverMoveEvent(QHoverEvent * event) override;
  void geometryChanged(const QRectF & newGeometry, const QRectF & oldGeometry) override;

private slots:
  void handleWindowChanged(QQuickWindow * window);
  void connectImages();

private:
  QRectF imageBounds(ImageDocument const & img) const;
  void reposition(ImageDocument & img);

  QColor color_;
  ImageCollection * images_;
  std::unique_ptr<ImageRenderer> renderer_;
  QPointF mousePosition_;
};

}