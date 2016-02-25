#ifndef __UTIL_H_
#define __UTIL_H_

#include <stdio.h>
#include <math3d.h>
#include <GLTools.h>


#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

class Util
{
public:

	static void printMaxtrix44f(const M3DMatrix44f matrix);

	// Load a TGA as a 2D Texture. Completely initialize the state
	static bool LoadTGATexture(const char *szFileName, GLenum minFilter, GLenum magFilter, GLenum wrapMode);

	static LPCSTR SclGetAppCurDir();
};
#endif