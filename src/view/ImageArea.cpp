#include <view/ImageArea.hpp>

namespace hdrv {

ImageArea::ImageArea()
  : images_(nullptr)
{
  connect(this, SIGNAL(windowChanged(QQuickWindow*)), this, SLOT(handleWindowChanged(QQuickWindow*)));
  setAcceptedMouseButtons(Qt::AllButtons);
  setAcceptHoverEvents(true);
}

void ImageArea::setModel(ImageCollection * images)
{
  if (images_ != images) {
    if (images_) {
      disconnect(images_, 0, this, 0);
    }
    images_ = images;
    connect(images_, SIGNAL(itemsChanged()),this, SLOT(connectImages()));
    connect(images_, SIGNAL(currentChanged()), window(), SLOT(update()));
    connectImages();
    emit modelChanged();
  }
}

void ImageArea::setColor(QColor const& color)
{
  color_ = color;
  emit colorChanged();
  if (window()) {
    window()->update();
  }
}

RenderRegion calculateRenderRegion(QSize windowSize, QPointF position, qreal width, qreal height, qreal ratio)
{
  int w = int(ratio * width);
  int h = int(ratio * height);
  int x = int(ratio * position.x());
  int y = int(ratio * (windowSize.height() - position.y() - height));
  return { QPoint(x, y), QSize(w, h) };
}

void ImageArea::sync()
{
  if (images_) {
    if (!renderer_) {
      renderer_ = std::make_unique<ImageRenderer>();
      connect(window(), SIGNAL(beforeRendering()), renderer_.get(), SLOT(paint()), Qt::DirectConnection);
    }
    auto & img = *images_->current();
    auto pos = mapToItem(nullptr, position());
    qreal ratio = window()->devicePixelRatio();
    renderer_->setRenderRegion(calculateRenderRegion(window()->size(), pos, width(), height(), ratio));
    renderer_->setClearColor(color_);
    renderer_->updateImages(images_->vector());
    renderer_->setCurrent(img.image());
    renderer_->setSettings({ QVector2D(img.position()), img.scale(), (float)img.brightness(), (float)img.gamma() });
  }
}

void ImageArea::mousePressEvent(QMouseEvent * event)
{
  if (event->button() == Qt::MouseButton::LeftButton) {
    setCursor(Qt::ClosedHandCursor);
    mousePosition_ = event->localPos();
  }
}

void ImageArea::mouseReleaseEvent(QMouseEvent * event)
{
  if (event->button() == Qt::MouseButton::LeftButton) {
    unsetCursor();
  }
}

void ImageArea::mouseMoveEvent(QMouseEvent * event)
{
  if (images_ && event->buttons() & Qt::MouseButton::LeftButton) {
    images_->current()->move(mousePosition_ - event->localPos());
    mousePosition_ = event->localPos();
  }
}

void ImageArea::wheelEvent(QWheelEvent * event)
{
  if (images_) {
    int step = event->angleDelta().y() > 0 ? 1 : -1;
    images_->current()->setScaleIndex(images_->current()->scaleIndex() + step);
  }
}

void ImageArea::hoverMoveEvent(QHoverEvent * event)
{
  if (images_ && images_->current()) {
    auto & img = *images_->current();
    QRectF bounds(QPointF(0, 0), QSizeF(img.width() * img.scale(), img.height() * img.scale()));
    bounds.moveTo(0.5 * QPointF(width(), height()) - 0.5 * img.scale() * QPointF(img.width(), img.height()));
    bounds.translate(-img.position());
    if (bounds.contains(event->posF())) {
      QPointF positionOnImage = (event->posF() - bounds.topLeft()) / img.scale();
      images_->current()->setCurrentPixel(QPoint(positionOnImage.x(), positionOnImage.y()));
    }
  }
}

void ImageArea::handleWindowChanged(QQuickWindow * window)
{
  if (window) {
    connect(window, SIGNAL(beforeSynchronizing()), this, SLOT(sync()), Qt::DirectConnection);
    connect(window, SIGNAL(sceneGraphInvalidated()), this, SLOT(cleanup()), Qt::DirectConnection);
    window->setClearBeforeRendering(false);
  }
}

void ImageArea::connectImages()
{
  for (auto image : images_->vector()) {
    connect(image, SIGNAL(propertyChanged()), window(), SLOT(update()), Qt::UniqueConnection);
  }
}

}
