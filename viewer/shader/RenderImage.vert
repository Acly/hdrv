#version 330

attribute highp vec2 vertices;
varying highp vec2 coords;

void main()
{
  gl_Position = vec4(vertices, 0, 1);
  coords = vertices * 0.5 + 0.5;
}
