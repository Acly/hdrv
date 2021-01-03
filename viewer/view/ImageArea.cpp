#include <view/ImageArea.hpp>

#include <cmath>

namespace hdrv {

ImageArea::ImageArea()
  : images_(nullptr)
{
  connect(this, &QQuickItem::windowChanged, this, &ImageArea::handleWindowChanged);
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
    connect(images_, &ImageCollection::itemsChanged,this, &ImageArea::connectImages);
    connect(images_, &ImageCollection::currentChanged, window(), &QQuickWindow::update);
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
      connect(window(), &QQuickWindow::beforeRendering, renderer_.get(), &ImageRenderer::init, Qt::DirectConnection);
      connect(window(), &QQuickWindow::beforeRenderPassRecording, renderer_.get(), &ImageRenderer::paint, Qt::DirectConnection);
    }
    auto & img = *images_->current();
    auto pos = mapToItem(nullptr, position());
    qreal ratio = window()->devicePixelRatio();
    renderer_->setRenderRegion(calculateRenderRegion(window()->size(), pos, width(), height(), ratio));
    renderer_->setClearColor(color_);
    renderer_->updateImages(images_->vector());
    renderer_->setCurrent(img.image(), img.layer());
    renderer_->setSettings({ QVector2D(img.position()), img.scale(), (float)img.brightness(), (float)img.gamma(), img.alphaMode() });
    renderer_->setComparison(img.comparison());
    renderer_->setWindow(window());
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
      newPos.setX(std::floor(wsize.width() / 2 - bounds.width() / 2));
    } else if (bounds.right() < wsize.width()) {
      newPos.setX(std::floor(-wsize.width() / 2 + bounds.width() / 2));
    }
  }
  if (bounds.height() <= height()) {
    newPos.setY(0);
  } else {
    if (bounds.top() > 0) {
      newPos.setY(std::floor(wsize.height() / 2 - bounds.height() / 2));
    } else if (bounds.bottom() < wsize.height()) {
      newPos.setY(std::floor(-wsize.height() / 2 + bounds.height() / 2));
    }
  }

  img.setPosition(newPos);
}

void ImageArea::updateComparisonSeparator(ImageDocument & img, QPointF pos)
{
  if (img.isComparison() && img.comparisonMode() == ComparisonMode::SideBySide) {
    auto bounds = imageBounds(img);
    float s = (pos.x() - bounds.left()) / bounds.width();
    img.setComparisonSeparator(s);
  }
}

void ImageArea::mousePressEvent(QMouseEvent * event)
{
  if (!images_) return;

  if (event->button() == Qt::MouseButton::LeftButton) {
    setCursor(Qt::ClosedHandCursor);
    mousePosition_ = event->position();
  } else if (event->button() == Qt::MouseButton::RightButton && images_) {
    auto & img = *images_->current();
    img.setComparisonMode(ComparisonMode::SideBySide);
    setCursor(Qt::SplitHCursor);
    updateComparisonSeparator(*images_->current(), event->position());
  }
}

void ImageArea::mouseReleaseEvent(QMouseEvent *)
{
  unsetCursor();
}

void ImageArea::mouseMoveEvent(QMouseEvent * event)
{
  if (images_) {
    auto & img = *images_->current();

    if (event->buttons() & Qt::MouseButton::LeftButton) {
      img.move(mousePosition_ - event->position());
      mousePosition_ = event->position();
      reposition(img);
    }
    if (event->buttons() & Qt::MouseButton::RightButton) {
      updateComparisonSeparator(img, event->position());
    }
  }
}

void ImageArea::wheelEvent(QWheelEvent * event)
{
  if (images_) {
    float mul = event->angleDelta().y() > 0 ? 2.0 : 0.5;

    auto & img = *images_->current();
    auto bounds = imageBounds(img);
    if (bounds.contains(event->position())) {
      int cx = bounds.center().x();
      int cy = bounds.center().y();
      QPointF pos = (event->position() - QPointF(cx, cy));
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
	const auto & bounds = imageBounds(img);
    if (bounds.contains(event->position())) {
      QPointF positionOnImage = (event->position() - bounds.topLeft()) / img.scale();
      images_->current()->setCurrentPixel(QPoint(positionOnImage.x(), positionOnImage.y()));
    }
  }
}

void ImageArea::geometryChange(const QRectF & newGeometry, const QRectF & oldGeometry)
{
  if (images_) {
    reposition(*images_->current());
  }
  QQuickItem::geometryChange(newGeometry, oldGeometry);
}

void ImageArea::handleWindowChanged(QQuickWindow * window)
{
  if (window) {
    connect(window, &QQuickWindow::beforeSynchronizing, this, &ImageArea::sync, Qt::DirectConnection);
    connect(window, &QQuickWindow::sceneGraphInvalidated, this, &ImageArea::cleanup, Qt::DirectConnection);
  }
}

void ImageArea::connectImages()
{
  for (auto image : images_->vector()) {
    connect(image, &ImageDocument::propertyChanged, window(), &QQuickWindow::update, Qt::UniqueConnection);
  }
}

void ImageArea::cleanup()
{
  renderer_ = nullptr;
}

struct CleanupJob : public QRunnable
{
  std::unique_ptr<ImageRenderer> renderer;
  CleanupJob(std::unique_ptr<ImageRenderer>&& r) : renderer(std::move(r)) {}
  void run() override { renderer.reset(); }
};

void ImageArea::releaseResources()
{
  window()->scheduleRenderJob(new CleanupJob(std::move(renderer_)), QQuickWindow::BeforeSynchronizingStage);
}

}
