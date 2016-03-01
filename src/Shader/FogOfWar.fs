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
    vec4 tmp = screenToWorldMatrix* vec4(x_dc,y_dc,z_dc,1);
    vec3 world_pos = tmp.xyz / tmp.w;
    if (world_pos.x > 0 && world_pos.x < 20 && world_pos.z > 0 && world_pos.z < 20){

    	float dist = distance(vec2(world_pos.x,world_pos.z),vec2(10,10));
    	if (dist < 2)
    	{
    		vFragColor = normal;  
    	}
    	else
    	{
    		vec4 FogColor = vec4(0.002750,0.026612,0.082870,0.08);
    		vFragColor = mix(normal, FogColor, 0.67);
    	}
    	
    }
    else
    	vFragColor = normal;   

}