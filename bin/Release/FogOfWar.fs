// The TexturedIdentity Shader
// Fragment Shader
// Richard S. Wright Jr.
// OpenGL SuperBible
#version 130

uniform mat4 screenToWorldMatrix;
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
      float z_dc = 1-2*z;
      float x_dc = 2*vVaryingTexCoords.s - 1;
      float y_dc = 2*vVaryingTexCoords.t - 1;
      vec4 world_pos = screenToWorldMatrix* vec4(x_dc,y_dc,z_dc,1);
      vec3 FogOFWarUV = world_pos.xyz / world_pos.w;
      if (FogOFWarUV.x > 0 && FogOFWarUV.x < 20 && FogOFWarUV.z > 0 && FogOFWarUV.z < 20)
        vFragColor.rgb = vec3(0); 
      else
        vFragColor.rgb = vec3(z); 
  }
}