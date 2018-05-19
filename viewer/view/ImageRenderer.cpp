#include <view/ImageRenderer.hpp>

#include <QFile>
#include <QTextStream>
#include <cmath>

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
  program->addShaderFromSourceCode(QOpenGLShader::Vertex, readFile(":/hdrv/shader/RenderImage.vert"));
  program->addShaderFromSourceCode(QOpenGLShader::Fragment, readFile(":/hdrv/shader/RenderImage.frag"));
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

QOpenGLTexture::PixelFormat pixelFormat(Image const& image)
{
  switch (image.channels()) {
    case 1: return QOpenGLTexture::Luminance;
    case 3: return QOpenGLTexture::RGB;
    case 4: return QOpenGLTexture::RGBA;
    default:
      qWarning() << "Cannot render image with " << image.channels() << " channels.";
      return QOpenGLTexture::Luminance;
  }
}

QOpenGLTexture::PixelType pixelType(Image const& image)
{
  return image.format() == Image::Float ? QOpenGLTexture::PixelType::Float32 : QOpenGLTexture::PixelType::UInt8;
}

std::unique_ptr<QOpenGLTexture> createTexture(Image const& image)
{
  auto texture = std::make_unique<QOpenGLTexture>(QOpenGLTexture::Target2D);
  texture->setSize(image.width(), image.height());
  texture->setFormat(format(image));
  texture->allocateStorage(pixelFormat(image), pixelType(image));
  texture->setData(pixelFormat(image), pixelType(image), image.data());
  texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
  texture->setMagnificationFilter(QOpenGLTexture::Nearest);
  texture->setWrapMode(QOpenGLTexture::ClampToBorder);
  texture->generateMipMaps();
  if (image.channels() == 1) {
    texture->setSwizzleMask(QOpenGLTexture::RedValue, QOpenGLTexture::RedValue,
                            QOpenGLTexture::RedValue, QOpenGLTexture::OneValue);
  }
  return texture;
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
    if (!tex) {
      tex = createTexture(*image);
    }
  };
  for (auto doc : images) {
    createTextureFor(doc->image());
    if (auto const& c = doc->comparison()) {
      createTextureFor(c->image);
    }
  }
}

void ImageRenderer::paint()
{
  if (!program_) {
    initializeOpenGLFunctions();
    program_ = createProgram();
  }

  program_->bind();
  program_->enableAttributeArray(0);
  program_->setAttributeArray(0, GL_FLOAT, vertexData, 2);

  auto const& region = renderRegion_;
  auto const& image = *current_;
  auto & texture = *textures_[current_];
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
  if (comparison_) {
    textures_[comparison_->image]->bind(1);
    program_->setUniformValue("comparison", 1);
    program_->setUniformValue("mode", (int)comparison_->mode);
    program_->setUniformValue("separator", comparison_->separator);
  } else {
    program_->setUniformValue("mode", -1);
  }

  glViewport(region.offset.x(), region.offset.y(), region.size.width(), region.size.height());

  glDisable(GL_DEPTH_TEST);
  glClearColor(clearColor_.red(), clearColor_.green(), clearColor_.blue(), 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  texture.release(0);
  program_->disableAttributeArray(0);
  program_->release();
}

}
