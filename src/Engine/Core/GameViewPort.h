#ifndef __GAMEVIEWPORT_H_
#define __GAMEVIEWPORT_H_

#include <GLTools.h>
#include <GLShaderManager.h>
#include <GLFrustum.h>
#include <GLBatch.h>
#include <GLFrame.h>
#include <GLMatrixStack.h>
#include <GLGeometryTransform.h>

#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

class GameViewPort
{

public:

	GameViewPort();

	void onChangeSize(int nWidth, int nHeight);

	//获取 投影矩阵
	const M3DMatrix44f& GetProjectionMatrix(void);

	int getWindowWidth();

	int getWindowHeight();

protected:
	int window_width, window_height;
	GLFrustum			viewFrustum;			// View Frustum
};

#endif