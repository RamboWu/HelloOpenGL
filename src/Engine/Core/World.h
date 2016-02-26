#ifndef __WORLD_H_
#define __WORLD_H_

#include <GLFrame.h>
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

#define NUM_SPHERES 50

class GameViewPort;

class World
{
public:
	World();

	virtual void update();

	void init();

	void draw();

	void destroy();

	void onChangeSize(int nWidth, int nHeight);

	GameViewPort* getGameViewPort();

protected:

	GameViewPort		*m_game_view_port;
	GLTriangleBatch		torusBatch;
	GLBatch				floorBatch;
	GLTriangleBatch     sphereBatch;

	GLFrame spheres[NUM_SPHERES];
	GLShaderManager		shaderManager;			// Shader Manager
};
#endif