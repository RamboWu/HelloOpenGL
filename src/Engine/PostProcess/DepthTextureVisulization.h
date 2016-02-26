#ifndef __DEPTHTEXTUREVISULIZATION_H_
#define __DEPTHTEXTUREVISULIZATION_H_

#include <GLTools.h>
#include <GLShaderManager.h>
#include <GLFrustum.h>
#include <GLBatch.h>


#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

#include "PostProcessRender.h"

class DepthTextureVisulization : public PostProcessRender
{

public:

	virtual PostProcessRender* init() override;

	virtual void render() override;

	virtual void destroy() override;

	virtual void onChangeSize(int nWidth, int nHeight) override;

private:

	GLuint				pixBuffObjs[1];
	GLuint				pixelDataSize;
	GLBatch             screenQuad;
	M3DMatrix44f        orthoMatrix;

	GLint				myTexturedIdentityShader;
	GLuint				textureID;

	GLuint				depthTextureID;
};

#endif