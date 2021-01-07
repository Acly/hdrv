#include <model/ImageDocument.hpp>

#include <QtConcurrent>
#include <QDir>
#include <QFileInfo>
#include <QFutureWatcher>
#include <QFileSystemWatcher>
#include <QSettings>
#include <QTimer>

#include <model/ImageCollection.hpp>

#include <algorithm>

namespace hdrv {

std::shared_ptr<Image> createDefaultImage()
{
  return std::make_shared<Image>(Image::makeEmpty());
}

QUrl defaultUrl() { return QUrl("file:////HDRV"); }
QString nameFromUrl(QUrl const& url) { return QFileInfo(url.fileName()).completeBaseName(); }

ImageDocument::ImageDocument(QUrl const& url, QObject * parent)
  : QObject(parent)
  , name_(nameFromUrl(url))
  , url_(url)
  , image_(createDefaultImage())
{
  init();

  watcher_ = setupWatcher(url_, false);
  load(url_.toLocalFile(), watcher_);
}

ImageDocument::ImageDocument(QUrl const& base, QUrl const& comparison, QObject * parent)
  : QObject(parent)
  , name_(nameFromUrl(base) + " | " + nameFromUrl(comparison))
  , url_(base)
  , comparisonUrl_(comparison)
  , image_(createDefaultImage())
{
  init();

  watcher_ = setupWatcher(url_, false);
  comparisonWatcher_ = setupWatcher(comparisonUrl_, true);

  load(url_.toLocalFile(), watcher_);
  load(comparisonUrl_.toLocalFile(), comparisonWatcher_);
}

ImageDocument::ImageDocument(QObject * parent)
  : QObject(parent)
  , url_(defaultUrl())
  , image_(createDefaultImage())
{
  init();
}

void ImageDocument::init()
{
  QSettings settings;
  displayMode_ = (DisplayMode)settings.value("Rendering/DisplayMode").toInt();
}

QFutureWatcher<ImageDocument::LoadResult>* ImageDocument::setupWatcher(QUrl const& url, bool comparison)
{
  QFutureWatcher<LoadResult>* watcher = new QFutureWatcher<LoadResult>(this);
  connect(watcher, SIGNAL(started()), this, SIGNAL(busyChanged()));
  connect(watcher, SIGNAL(finished()), this, SIGNAL(busyChanged()));

  connect(
    watcher, &QFutureWatcher<LoadResult>::finished,
    std::bind(&ImageDocument::loadFinished, this, watcher, url, comparison));

  auto* fsWatcher = new QFileSystemWatcher(QStringList(url.toLocalFile()), this);
  auto* delay = new QTimer(this);
  delay->setSingleShot(true);

  connect(
    fsWatcher, &QFileSystemWatcher::fileChanged,
    [delay]() { delay->start(500); });
  connect(
    delay, &QTimer::timeout,
    std::bind(&ImageDocument::load, this, url.toLocalFile(), watcher));

  return watcher;
}

QUrl ImageDocument::directory() const
{
  return QUrl::fromLocalFile(QFileInfo(url().toLocalFile()).absolutePath());
}

QString ImageDocument::fileType() const
{
  if (isDefault()) {
    return "Default";
  } else if (isComparison()) {
    return "Comparison";
  } else if (isFloat()) {
    auto ext = QFileInfo(url().fileName()).suffix();
    if (ext == "exr") {
      return "FormatEXR";
    } else if (ext == "pfm" || ext == "ppm") {
      return "FormatPFM";
    } else {
      return "FormatHDR";
    }
  } else { // not float
    return "FormatByte";
  }
}

int ImageDocument::channels() const
{ // clamp layer_ since QML combo sets it to -1 if it is not populated
  return image_->channels(std::clamp(layer_, 0, int(image_->layers().size())));
}

QList<QString> ImageDocument::layers() const
{
  QList<QString> list;
  for (auto&& layer : image_->layers()) {
    if (layer.name.empty()) {
      list.push_back(QString("Default"));
    } else {
      list.push_back(QString::fromStdString(layer.name));
    }
  } 
  if (image_->layers().empty()) {
    list.push_back(QString("Default"));
  }
  return list;
}

bool ImageDocument::isDefault() const { return url() == defaultUrl(); }

void ImageDocument::resetError()
{
  for (int i = 0; i < errorText_.size(); ++i) {
    errorText_[i] = "";
  }
  emit errorTextChanged();
}

void ImageDocument::setError(QString const& errorText, ErrorCategory category)
{
  errorText_[static_cast<int>(category)] = errorText;
  emit errorTextChanged();
}

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

void ImageDocument::setDisplayMode(DisplayMode displayMode)
{
  if (displayMode_ != displayMode) {
    displayMode_ = displayMode;

    QSettings settings;
    settings.setValue("Rendering/DisplayMode", QVariant((int)displayMode));

    emit displayModeChanged();
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

void ImageDocument::setComparisonMode(ComparisonMode mode)
{
  if (comparison_ && comparison_->mode != mode) {
    comparison_->mode = mode;
    emit comparisonModeChanged();
    emit propertyChanged();
  }
}

void ImageDocument::setComparisonSeparator(float value)
{
  if (comparison_ && !qFuzzyCompare(comparison_->separator, value)) {
    comparison_->separator = value;
    emit comparisonSeparatorChanged();
    emit propertyChanged();
  }
}

void ImageDocument::setLayer(int layer)
{
  if (layer_ != layer) {
    layer_ = layer;
    emit layerChanged();
    emit propertyChanged();
  }
}

void ImageDocument::store(QUrl const& url)
{
  QFileInfo file(url.toLocalFile());
  if (file.suffix() == "hdr" || file.suffix() == "pic") {
    check(image()->storePIC(file.absoluteFilePath().toStdString()), ErrorCategory::Generic);
  } else if (file.suffix() == "pfm" || file.suffix() == "ppm") {
    check(image()->storePFM(file.absoluteFilePath().toStdString()), ErrorCategory::Generic);
  } else if (file.suffix() == "exr") {
    check(image()->storeEXR(file.absoluteFilePath().toStdString()), ErrorCategory::Generic);
  } else if (file.suffix() == "png") {
    check(image()->storeImage(file.absoluteFilePath().toStdString(), pow(2.0f, brightness()), 1.0f / gamma()), ErrorCategory::Generic);
  } else {
    setError("Unsupported file extension: " + file.suffix(), ErrorCategory::Generic);
  }
}

QVector4D ImageDocument::pixelValue() const
{
  QVector4D texel;
  int l = std::clamp(layer_, 0, int(image_->layers().size()));
  texel.setX(image_->value(pixelPosition_.x(), pixelPosition_.y(), 0, l));
  if (channels() > 1) {
    texel.setY(image_->value(pixelPosition_.x(), pixelPosition_.y(), 1, l));
    if (channels() > 2) {
      texel.setZ(image_->value(pixelPosition_.x(), pixelPosition_.y(), 2, l));
      if (channels() > 3) {
        texel.setW(image_->value(pixelPosition_.x(), pixelPosition_.y(), 3, l));
      }
    }
  }
  if (image_->layers().size() > l && image_->layers()[l].display == Image::Integer) {
    for (int i = 0; i < channels(); ++i) {
      uint32_t integer;
      std::memcpy(&integer, &texel[i], sizeof(float));
      texel[i] = float(integer);
    }
  }
  return texel;
}

void ImageDocument::load(QString const& path, QFutureWatcher<LoadResult>* watcher)
{
  QFuture<LoadResult> future = QtConcurrent::run([path]() {
    QFileInfo file(path);
    std::string path = file.absoluteFilePath().toStdString();
    if (!file.exists()) {
      return std::make_shared<Result<Image>>("File " + path + " does not exist.");
    }
    if (file.suffix() == "hdr" || file.suffix() == "pic") {
      return std::make_shared<Result<Image>>(Image::loadPIC(path));
    } else if (file.suffix() == "pfm" || file.suffix() == "ppm") {
      return std::make_shared<Result<Image>>(Image::loadPFM(path));
    } else if (file.suffix() == "exr") {
      return std::make_shared<Result<Image>>(Image::loadEXR(path));
    } else {
      return std::make_shared<Result<Image>>(Image::loadImage(path));
    }
  });
  watcher->setFuture(future);
}

void ImageDocument::loadFinished(QFutureWatcher<LoadResult>* watcher, QUrl const& url, bool comparison)
{
  auto result = watcher->result();
  if (check(*result, comparison ? ErrorCategory::Comparison : ErrorCategory::Image, "Failed to load " + url.toLocalFile() + ": ")) {
    if (!comparison) {
      image_ = std::make_shared<Image>(std::move(*result).value());
    } else {
      comparison_ = Comparison(std::make_shared<Image>(std::move(*result).value()));
      emit isComparisonChanged();
    }
    setError("", comparison ? ErrorCategory::Comparison : ErrorCategory::Image);
    emit errorTextChanged();
    emit fileTypeChanged();
    emit propertyChanged();
  }
}

}
