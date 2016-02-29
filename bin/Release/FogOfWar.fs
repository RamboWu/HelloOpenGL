// The TexturedIdentity Shader
// Fragment Shader
// Richard S. Wright Jr.
// OpenGL SuperBible
#version 130


uniform sampler2D colorMap;
uniform sampler2D depthTexture;

out vec4 vFragColor;
smooth in vec2 vVaryingTexCoords;
 
void main() {
 
  if(vVaryingTexCoords.s < 0.3) {
    vFragColor = texture(colorMap, vVaryingTexCoords.st);
  }
  else if (vVaryingTexCoords.s < 0.6)
  {
 
    float z = texture(depthTexture, vVaryingTexCoords.st).r;
    float n = 1.0;
    float f = 30.0;
    float c = (2.0 * n) / (f + n - z * (f - n));
 
    vFragColor.rgb = vec3(c); 
  }
  else
  {
      float z = texture(depthTexture, vVaryingTexCoords.st).r;
      vFragColor.rgb = vec3(z); 
  }
}