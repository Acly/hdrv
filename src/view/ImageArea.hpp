#pragma once

#include <memory>

#include <QQuickItem>
#include <QQuickWindow>

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
  void setModel(ImageCollection * v);

  QColor const& color() const { return color_; }
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

private slots:
  void handleWindowChanged(QQuickWindow * window);
  void connectImages();

private:
  QColor color_;
  ImageCollection * images_;
  std::unique_ptr<ImageRenderer> renderer_;
  QPointF mousePosition_;
};

}