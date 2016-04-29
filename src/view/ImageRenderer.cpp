#include <view/ImageRenderer.hpp>

char const* vertexSource = R"(
#version 330

attribute highp vec2 vertices;
varying highp vec2 coords;

void main()
{
  gl_Position = vec4(vertices, 0, 1);
  coords = vertices * 0.5 + 0.5;
})";

char const* fragmentSource = R"(
#version 330

uniform sampler2D tex;
uniform sampler2D comparison;

uniform vec2 position;
uniform vec2 scale;
uniform vec2 regionSize;
uniform float gamma;
uniform float brightness;
uniform int mode;

varying highp vec2 coords;

void main()
{
  vec2 pos = (coords - position) / scale;
  if(pos.x >= 0.0 && pos.x <= 1.0 && pos.y >= 0.0 && pos.y <= 1.0) {
    vec3 checker = (int(floor(0.1*coords.x*regionSize.x) + floor(0.1*coords.y*regionSize.y)) & 1) > 0 ? vec3(0.4) : vec3(0.6);
    vec4 texel = texture2D(tex, pos);

    if (mode == 1) { // difference
      vec4 comp = texture2D(comparison, pos);
      texel = vec4(abs(comp - texel).xyz, 1.0);
    }

    vec3 color = pow(brightness * texel.xyz, vec3(gamma));
    gl_FragColor = vec4(mix(checker, color.xyz, texel.w), 1.0);
  } else {
    gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
  }
})";

float const vertexData[] = {-1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f};

std::unique_ptr<QOpenGLShaderProgram> createProgram()
{
  auto program = std::make_unique<QOpenGLShaderProgram>();
  program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexSource);
  program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentSource);
  program->bindAttributeLocation("vertices", 0);
  program->link();
  return program;
}

namespace hdrv {

QOpenGLTexture::TextureFormat format(Image const& image)
{
  return image.format() == Image::Float ?
    (image.channels() == 3 ? QOpenGLTexture::RGB32F : QOpenGLTexture::RGBA32F) :
    (image.channels() == 3 ? QOpenGLTexture::RGBFormat : QOpenGLTexture::RGBAFormat);
}

QOpenGLTexture::PixelFormat pixelFormat(Image const& image)
{
  return image.channels() == 3 ? QOpenGLTexture::RGB : QOpenGLTexture::RGBA;
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
  return texture;
}

QVector2D texturePosition(QVector2D regionSize, QVector2D imageSize, QVector2D imagePosition)
{
  auto offset = (regionSize - imageSize) / 2.0f;
  return (offset + QVector2D(-imagePosition.x(), imagePosition.y())) / regionSize;
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
  program_->setUniformValue("brightness", pow(2.0f, settings_.brightness));
  program_->setUniformValue("gamma", current_->format() == Image::Float ? 1.0f / settings_.gamma : 1.0f);
  if (comparison_) {
    textures_[comparison_->image]->bind(1);
    program_->setUniformValue("comparison", 1);
    program_->setUniformValue("mode", 1);
  } else {
    program_->setUniformValue("mode", 0);
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
