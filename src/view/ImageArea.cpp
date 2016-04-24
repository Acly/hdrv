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

QRectF ImageArea::imageBounds(ImageDocument const & img) const
{
  QRectF bounds(QPointF(0, 0), QSizeF(img.width() * img.scale(), img.height() * img.scale()));
  bounds.moveTo(0.5 * QPointF(width(), height()) - 0.5 * img.scale() * QPointF(img.width(), img.height()));
  bounds.translate(-img.position());
  return bounds;
}

void ImageArea::reposition(ImageDocument & img)
{
  QSizeF wsize = QSizeF(width(), height());
  QRectF bounds = imageBounds(img);

  QPointF newPos = img.position();
  if (bounds.width() <= wsize.width()) {
    newPos.setX(0);
  } else {
    if (bounds.left() > 0) {
      newPos.setX(wsize.width() / 2 - bounds.width() / 2);
    } else if (bounds.right() < wsize.width()) {
      newPos.setX(-wsize.width() / 2 + bounds.width() / 2);
    }
  }
  if (bounds.height() <= height()) {
    newPos.setY(0);
  } else {
    if (bounds.top() > 0) {
      newPos.setY(wsize.height() / 2 - bounds.height() / 2);
    } else if (bounds.bottom() < wsize.height()) {
      newPos.setY(-wsize.height() / 2 + bounds.height() / 2);
    }
  }

  img.setPosition(newPos);
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
    reposition(*images_->current());
  }
}

void ImageArea::wheelEvent(QWheelEvent * event)
{
  if (images_) {
    float mul = event->angleDelta().y() > 0 ? 1.25 : 0.8;

    auto & img = *images_->current();
    auto & bounds = imageBounds(img);
    if (bounds.contains(event->posF())) {
      QPointF pos = (event->posF() - bounds.center());
      QPointF newPos = mul * pos;

      img.move(newPos - pos);
    }

    float scale = img.scale() * mul;
    img.setScale(scale);

    reposition(*images_->current());
  }
}

void ImageArea::hoverMoveEvent(QHoverEvent * event)
{
  if (images_ && images_->current()) {
    auto & img = *images_->current();
    auto & bounds = imageBounds(img);
    if (bounds.contains(event->posF())) {
      QPointF positionOnImage = (event->posF() - bounds.topLeft()) / img.scale();
      images_->current()->setCurrentPixel(QPoint(positionOnImage.x(), positionOnImage.y()));
    }
  }
}

void ImageArea::geometryChanged(const QRectF & newGeometry, const QRectF & oldGeometry)
{
  if (images_) {
    reposition(*images_->current());
  }
  QQuickItem::geometryChanged(newGeometry, oldGeometry);
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
