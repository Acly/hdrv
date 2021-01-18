#version 330

uniform sampler2D tex;
uniform sampler2D comparison;

uniform vec2 position;
uniform vec2 scale;
uniform vec2 regionSize;
uniform float gamma;
uniform float brightness;
uniform int display;
uniform int mode;
uniform float separator;

varying highp vec2 coords;

#define Difference 0
#define SideBySide 1

void main()
{
  vec2 pos = (coords - position) / scale;
  if(pos.x >= 0.0 && pos.x <= 1.0 && pos.y >= 0.0 && pos.y <= 1.0) {
    vec3 checker = (int(floor(0.1*coords.x*regionSize.x) + floor(0.1*coords.y*regionSize.y)) & 1) > 0 ? vec3(0.4) : vec3(0.6);
    vec4 texel = texture2D(tex, pos);

    if (mode == Difference) {
      vec4 comp = texture2D(comparison, pos);
      texel = vec4(abs(comp - texel).xyz, 1.0);
    } else if (mode == SideBySide) {
      if (pos.x > separator) {
        texel = texture2D(comparison, pos);
      }
    }

    vec3 color = pow(brightness * texel.xyz, vec3(gamma));
    switch(display) {
      case 1: // No Alpha
        gl_FragColor = vec4(color.xyz, 1.0);
        break;
      case 2: // Alpha Only
        gl_FragColor = vec4(vec3(texel.w), 1.0);
        break;
      case 3: // Normal
        gl_FragColor = vec4(texel.xyz * 0.5 + 0.5, 1.0);
        break;
      case 4: // Integer
        gl_FragColor = vec4(floatBitsToUint(texel));
        break;
      default: // Default
        gl_FragColor = vec4(mix(checker, color.xyz, texel.w), 1.0);
        break;
    }
  } else {
    gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
  }
}
