// The TexturedIdentity Shader
// Fragment Shader
// Richard S. Wright Jr.
// OpenGL SuperBible
#version 130


uniform sampler2D colorMap;

out vec4 vFragColor;
smooth in vec2 vVaryingTexCoords;


void main(void)
{ 
   	vec4 vColor; 
   	vColor = texture(colorMap, vVaryingTexCoords.st);
    
   	float grey = dot(vColor.rgb, vec3(0.3, 0.59, 0.11));
	vFragColor = vec4(grey, grey, grey, 1.0f);

}