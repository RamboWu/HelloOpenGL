// The TexturedIdentity Shader
// Vertex Shader
// Richard S. Wright Jr.
// OpenGL SuperBible
#version 130

uniform mat4 mvpMatrix;

in vec4 vVertex;
in vec2 vTexCoords;

smooth out vec2 vVaryingTexCoords; 

void main(void) 
    { 
    vVaryingTexCoords = vTexCoords;
    gl_Position = mvpMatrix * vVertex;
    }