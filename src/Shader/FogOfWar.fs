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
 
  	vec4 normal;

    normal = texture(colorMap, vVaryingTexCoords.st);
 
    float z = texture(depthTexture, vVaryingTexCoords.st).r;
    float z_dc = 2*z - 1;
    float x_dc = 2*vVaryingTexCoords.s - 1;
    float y_dc = 2*vVaryingTexCoords.t - 1;
    vec4 world_pos = screenToWorldMatrix* vec4(x_dc,y_dc,z_dc,1);
    vec3 FogOFWarUV = world_pos.xyz / world_pos.w;
    if (FogOFWarUV.x > 0 && FogOFWarUV.x < 20 && FogOFWarUV.z > 0 && FogOFWarUV.z < 20){
    	vec4 FogColor = vec4(0.002750,0.026612,0.082870,0.08);
    	vFragColor = mix(normal, FogColor, 0.67);
    }
    else
    	vFragColor = normal;   

}