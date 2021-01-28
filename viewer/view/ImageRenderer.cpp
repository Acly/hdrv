#include <view/ImageRenderer.hpp>

#include <QFile>
#include <QOpenGLPixelTransferOptions>
#include <QTextStream>
#include <QQuickWindow>
#include <QSGRendererInterface>

#include <cmath>
#include <iostream>

float const vertexData[] = {-1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f};

QString readFile(QString const& name)
{
  QFile file(name);
  if (!file.open(QFile::ReadOnly | QFile::Text)) {
    qCritical() << "Cannot read shader source: " << name;
    return "";
  }
  QTextStream stream(&file);
  return stream.readAll();
}

std::unique_ptr<QOpenGLShaderProgram> createProgram()
{
  auto program = std::make_unique<QOpenGLShaderProgram>();
  program->addCacheableShaderFromSourceCode(QOpenGLShader::Vertex, readFile(":/hdrv/shader/RenderImage.vert"));
  program->addCacheableShaderFromSourceCode(QOpenGLShader::Fragment, readFile(":/hdrv/shader/RenderImage.frag"));
  program->bindAttributeLocation("vertices", 0);
  program->link();
  return program;
}

namespace hdrv {

QOpenGLTexture::TextureFormat format(Image const& image)
{
  switch (image.format()) {
    case Image::Float: {
      switch (image.channels()) {
        case 1: return QOpenGLTexture::R32F;
        case 3: return QOpenGLTexture::RGB32F;
        case 4: return QOpenGLTexture::RGBA32F;
        default: return QOpenGLTexture::RGBA32F;
      }
    }
    case Image::Byte: {
      switch (image.channels()) {
        case 1: return QOpenGLTexture::R8_UNorm;
        case 3: return QOpenGLTexture::RGBFormat;
        case 4: return QOpenGLTexture::RGBAFormat;
        default: return QOpenGLTexture::RGBAFormat;
      }
    }
    default: return QOpenGLTexture::RGBA32F;
  }
}

QOpenGLTexture::PixelFormat pixelFormat(int channelCount)
{
  switch (channelCount) {
    case 1: return QOpenGLTexture::Luminance;
    case 2: return QOpenGLTexture::RG;
    case 3: return QOpenGLTexture::RGB;
    case 4: return QOpenGLTexture::RGBA;
    default:
      qWarning() << "Cannot render image with " << channelCount << " channels.";
      return QOpenGLTexture::Luminance;
  }
}

QOpenGLTexture::PixelType pixelType(Image const& image)
{
  return image.format() == Image::Float ? QOpenGLTexture::PixelType::Float32 : QOpenGLTexture::PixelType::UInt8;
}

std::unique_ptr<QOpenGLTexture> createTexture(Image const& image, Image::Layer const& layer)
{
  QOpenGLPixelTransferOptions options;
  if (image.format() == Image::Byte) {
    options.setAlignment(1); // GL_UNPACK_ALIGNMENT
  }
  auto texture = std::make_unique<QOpenGLTexture>(QOpenGLTexture::Target2D);
  texture->setSize(image.width(), image.height());
  texture->setFormat(format(image));
  texture->allocateStorage(pixelFormat(layer.channels), pixelType(image));
  texture->setData(pixelFormat(layer.channels), pixelType(image), image.data() + layer.offset, &options);
  texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
  texture->setMagnificationFilter(QOpenGLTexture::Nearest);
  texture->setWrapMode(QOpenGLTexture::ClampToBorder);
  texture->generateMipMaps();
  if (layer.display == Image::Luminance || layer.display == Image::Depth) {
    texture->setSwizzleMask(QOpenGLTexture::RedValue, QOpenGLTexture::RedValue,
                            QOpenGLTexture::RedValue, QOpenGLTexture::OneValue);
  }
  return texture;
}

std::vector<std::unique_ptr<QOpenGLTexture>> createTextures(Image const& image)
{
  std::vector<std::unique_ptr<QOpenGLTexture>> result;
  if (image.layers().empty()) {
    auto display = image.channels() == 1 ? Image::Luminance : Image::Color;
    result.push_back(createTexture(image, Image::Layer{"", image.channels(), display, 0}));
  } else {
    for (auto& layer : image.layers()) {
      result.push_back(createTexture(image, layer));
    }
  }
  return result;
}

QVector2D texturePosition(QVector2D regionSize, QVector2D imageSize, QVector2D imagePosition)
{
  auto offset = (regionSize - imageSize) / 2.0f;
  auto roffset = QVector2D(std::floor(offset.x()), std::floor(offset.y()));
  return (roffset + QVector2D(-imagePosition.x(), imagePosition.y())) / regionSize;
}

QVector2D textureScale(QVector2D regionSize, QVector2D imageSize)
{
  return imageSize / regionSize;
}

void ImageRenderer::updateImages(std::vector<ImageDocument *> const& images)
{
  // Erase textures for images that no longer exist
  for (auto iter = textures_.begin(); iter != textures_.end(); ) {
    auto matchImage = [iter](ImageDocument * doc) {
      return doc->image() == iter->first
        || (doc->comparison() && doc->comparison()->image == iter->first);
    };
    if (std::find_if(images.begin(), images.end(), matchImage) == images.end()) {
      textures_.erase(iter++);
    } else {
      ++iter;
    }
  }
  // Create textures for new images
  auto createTextureFor = [this](std::shared_ptr<Image> const& image) {
    auto & tex = textures_[image];
    if (tex.empty()) {
      tex = createTextures(*image);
    }
  };
  for (auto doc : images) {
    createTextureFor(doc->image());
    if (auto const& c = doc->comparison()) {
      createTextureFor(c->image);
    }
  }
}

void ImageRenderer::init()
{
  if (!program_) {
    QSGRendererInterface* rif = window_->rendererInterface();
    Q_ASSERT(rif->graphicsApi() == QSGRendererInterface::OpenGL || rif->graphicsApi() == QSGRendererInterface::OpenGLRhi);

    initializeOpenGLFunctions();
    program_ = createProgram();
  }
}

QOpenGLTexture& ImageRenderer::findTexture(std::shared_ptr<Image> const& image, int layer)
{
  auto i = textures_.find(image);
  Q_ASSERT(i != textures_.end());
  if (i->second.size() == 1) {
    return *i->second[0];
  }   
  Q_ASSERT(layer < i->second.size());
  return *i->second[layer];
}

void ImageRenderer::paint()
{
  window_->beginExternalCommands();

  program_->bind();
  program_->enableAttributeArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  program_->setAttributeArray(0, GL_FLOAT, vertexData, 2);

  auto const& region = renderRegion_;
  auto const& image = *current_;
  auto& texture = findTexture(current_, layer_);
  QVector2D regionSize(float(region.size.width()), float(region.size.height()));
  QVector2D imageSize(float(image.width()) * settings_.scale, float(image.height()) * settings_.scale);

  texture.setBorderColor(clearColor_);
  texture.bind(0);
  program_->setUniformValue("tex", 0);
  program_->setUniformValue("position", texturePosition(regionSize, imageSize, settings_.position));
  program_->setUniformValue("scale", textureScale(regionSize, imageSize));
  program_->setUniformValue("regionSize", regionSize);
  program_->setUniformValue("brightness", std::pow(2.0f, settings_.brightness));
  program_->setUniformValue("gamma", current_->format() == Image::Float ? 1.0f / settings_.gamma : 1.0f);
  program_->setUniformValue("display", (int)settings_.displayMode);
  if (comparison_) {
    findTexture(comparison_->image, 0).bind(1);
    program_->setUniformValue("comparison", 1);
    program_->setUniformValue("mode", (int)comparison_->mode);
    program_->setUniformValue("separator", comparison_->separator);
  } else {
    program_->setUniformValue("mode", -1);
  }

  glViewport(region.offset.x(), region.offset.y(), region.size.width(), region.size.height());

  glDisable(GL_DEPTH_TEST);
  glClearColor(clearColor_.redF(), clearColor_.greenF(), clearColor_.blueF(), 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  texture.release(0);
  program_->disableAttributeArray(0);
  program_->release();

  window_->endExternalCommands();
}

}
