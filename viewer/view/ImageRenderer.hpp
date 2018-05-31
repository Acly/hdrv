#pragma once

#include <memory>

#include <QObject>
#include <QColor>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

#include <model/ImageCollection.hpp>

namespace hdrv {

struct RenderRegion
{
  QPoint offset;
  QSize size;
};

struct ImageSettings
{
  QVector2D position;
  float scale;
  float brightness;
  float gamma;
  ImageDocument::AlphaMode alphaMode;
};

class ImageRenderer : public QObject, protected QOpenGLFunctions
{
  Q_OBJECT

public:
  using ImageTextures = std::map<std::shared_ptr<Image>, std::unique_ptr<QOpenGLTexture>>;

  void setRenderRegion(RenderRegion region) { renderRegion_ = region; }
  void setClearColor(QColor color) { clearColor_ = color; }
  void setSettings(ImageSettings settings) { settings_ = settings; }
  void setCurrent(std::shared_ptr<Image> const& image) { current_ = image; }
  void setComparison(boost::optional<ImageComparison> const& c) { comparison_ = c; }
  void updateImages(std::vector<ImageDocument *> const& images);

public slots:
  void paint();
  
private:
  RenderRegion renderRegion_;
  QColor clearColor_;
  ImageSettings settings_;
  ImageTextures textures_;
  std::shared_ptr<Image> current_;
  boost::optional<ImageComparison> comparison_;
  std::unique_ptr<QOpenGLShaderProgram> program_;
};

}