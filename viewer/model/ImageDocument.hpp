#pragma once

#include <memory>
#include <optional>

#include <QList>
#include <QObject>
#include <QSize>
#include <QPoint>
#include <QUrl>
#include <QVector4D>
#include <QFutureWatcher>

#include <image/Image.hpp>

namespace hdrv {

class ImageDocument : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString name READ name CONSTANT FINAL)
  Q_PROPERTY(QUrl url READ url CONSTANT FINAL)
  Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
  Q_PROPERTY(QStringList errorText READ errorText NOTIFY errorTextChanged)
  Q_PROPERTY(QUrl directory READ directory CONSTANT FINAL)
  Q_PROPERTY(QString fileType READ fileType NOTIFY fileTypeChanged)
  Q_PROPERTY(QSize size READ size NOTIFY propertyChanged)
  Q_PROPERTY(int channels READ channels NOTIFY propertyChanged)
  Q_PROPERTY(QPointF position READ position WRITE setPosition NOTIFY positionChanged)
  Q_PROPERTY(float scale READ scale WRITE setScale NOTIFY scaleChanged)
  Q_PROPERTY(qreal brightness READ brightness WRITE setBrightness NOTIFY brightnessChanged)
  Q_PROPERTY(qreal minBrightness READ minBrightness CONSTANT FINAL)
  Q_PROPERTY(qreal maxBrightness READ maxBrightness CONSTANT FINAL)
  Q_PROPERTY(qreal gamma READ gamma WRITE setGamma NOTIFY gammaChanged)
  Q_PROPERTY(qreal minGamma READ minGamma CONSTANT FINAL)
  Q_PROPERTY(qreal maxGamma READ maxGamma CONSTANT FINAL)
  Q_PROPERTY(bool isFloat READ isFloat NOTIFY propertyChanged)
  Q_PROPERTY(AlphaMode alphaMode READ alphaMode WRITE setAlphaMode NOTIFY alphaModeChanged)
  Q_PROPERTY(QPoint pixelPosition READ pixelPosition NOTIFY pixelPositionChanged)
  Q_PROPERTY(QVector4D pixelValue READ pixelValue NOTIFY pixelValueChanged)
  Q_PROPERTY(bool isComparison READ isComparison NOTIFY isComparisonChanged)
  Q_PROPERTY(ComparisonMode comparisonMode READ comparisonMode WRITE setComparisonMode NOTIFY comparisonModeChanged)
  Q_PROPERTY(float comparisonSeparator READ comparisonSeparator WRITE setComparisonSeparator NOTIFY comparisonSeparatorChanged)
  Q_PROPERTY(bool hasLayers READ hasLayers NOTIFY propertyChanged)
  Q_PROPERTY(QList<QString> layers READ layers NOTIFY propertyChanged)
  Q_PROPERTY(int layer READ layer WRITE setLayer NOTIFY layerChanged)

public:
  enum class ComparisonMode { Difference, SideBySide };
  Q_ENUMS(ComparisonMode)
  
  enum class AlphaMode { Default, NoAlpha, AlphaOnly };
  Q_ENUMS(AlphaMode)

  struct Comparison
  {
    std::shared_ptr<Image> image;
    ComparisonMode mode = ComparisonMode::Difference;
    float separator = 0.5f;

    Comparison() = default;
    Comparison(std::shared_ptr<Image> i) : image(std::move(i)) {}
  };

  ImageDocument(QUrl const& url, QObject * parent = nullptr);
  ImageDocument(QUrl const& base, QUrl const& comparison, QObject * parent = nullptr);
  ImageDocument(QObject * parent = nullptr);

  void init();

  QString const& name() const { return name_; }
  QUrl const& url() const { return url_; }
  bool busy() const { return (watcher_ && watcher_->isRunning()) || (comparisonWatcher_ && comparisonWatcher_->isRunning()); }
  QStringList const& errorText() const { return errorText_; }
  QUrl directory() const;
  QString fileType() const;
  QPointF position() const { return position_; }
  int width() const { return image_->width();  }
  int height() const { return image_->height(); }
  QSize size() const { return QSize(image_->width(), image_->height()); }
  int channels() const;
  float scale() const { return scale_; }
  qreal brightness() const { return brightness_; }
  qreal minBrightness() const { return -10.0; }
  qreal maxBrightness() const { return 10.0; }
  qreal gamma() const { return image_->format() == Image::Float ? gamma_ : 2.2; }
  qreal minGamma() const { return 1.0; }
  qreal maxGamma() const { return 8.0; }
  bool isFloat() const { return image_->format() == Image::Float; }
  AlphaMode alphaMode() const { return alphaMode_; }
  void const* pixels() const { return image_->data(); }
  std::shared_ptr<Image> const& image() { return image_; }
  QPoint pixelPosition() const { return pixelPosition_; }
  QVector4D pixelValue() const;
  bool isDefault() const;
  bool isComparison() const { return (bool)comparison_; }
  std::optional<Comparison> const& comparison() const { return comparison_; }
  ComparisonMode comparisonMode() const { return comparison_.value_or(Comparison()).mode; }
  float comparisonSeparator() const { return comparison_.value_or(Comparison()).separator; }
  bool hasLayers() const { return image_->layers().size() > 1; }
  QList<QString> layers() const;
  int layer() const { return layer_; }

  enum class ErrorCategory { Image, Comparison, Generic };
  void setError(QString const& errorText, ErrorCategory category);

  void setPosition(QPointF pos);
  void move(QPointF offset);
  void setScale(qreal scale);
  void setBrightness(qreal brightness);
  void setGamma(qreal gamma);
  void setAlphaMode(AlphaMode alphaMode);
  void setCurrentPixel(QPoint index);
  void setComparisonMode(ComparisonMode mode);
  void setComparisonSeparator(float value);
  void setLayer(int layer);

  Q_INVOKABLE void resetError();
  Q_INVOKABLE void store(QUrl const& url);

signals:
  void busyChanged();
  void errorTextChanged();
  void propertyChanged();
  void positionChanged();
  void scaleChanged();
  void brightnessChanged();
  void gammaChanged();
  void alphaModeChanged();
  void pixelPositionChanged();
  void pixelValueChanged();
  void isComparisonChanged();
  void comparisonModeChanged();
  void comparisonSeparatorChanged();
  void fileTypeChanged();
  void layerChanged();

private:
  typedef std::shared_ptr<Result<Image>> LoadResult;
  QFutureWatcher<LoadResult>* setupWatcher(QUrl const& url, bool comparison);
  void load(QString const& path, QFutureWatcher<LoadResult>* watcher);
  void loadFinished(QFutureWatcher<LoadResult>* watcher, QUrl const& url, bool comparison);

  template<class T>
  bool check(Result<T> const& result, ErrorCategory category, QString const& prefix = "") {
    if (!result) {
      setError(prefix + QString::fromStdString(result.error()), category);
    }
    return (bool)result;
  }

  QString name_ = "HDRV";
  QUrl url_ ;
  QUrl comparisonUrl_;
  QStringList errorText_ = { "","","" };
  QPointF position_;
  qreal scale_ = 1.0f;
  qreal brightness_ = 0.0f;
  qreal gamma_ = 2.2f;
  AlphaMode alphaMode_ = AlphaMode::Default;
  QPoint pixelPosition_;
  QVector4D pixelValue_;
  std::shared_ptr<Image> image_;
  std::optional<Comparison> comparison_;
  int layer_ = 0;
  QFutureWatcher<LoadResult>* watcher_ = nullptr;
  QFutureWatcher<LoadResult>* comparisonWatcher_ = nullptr;
};

using ImageComparison = ImageDocument::Comparison;
using ComparisonMode = ImageDocument::ComparisonMode;

}

Q_DECLARE_METATYPE(hdrv::ImageDocument*);
