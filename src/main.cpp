//#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glew32s.lib")
//#pragma comment(lib, "freeglut.lib")

#include <GLTools.h>
#include <GLShaderManager.h>
#include <GLFrustum.h>
#include <GLBatch.h>
#include <GLFrame.h>
#include <GLMatrixStack.h>
#include <GLGeometryTransform.h>


#include <math.h>
#include <stdio.h>
#include "Util/Util.h"

#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

#include "Camera.h"
#include "World.h"
#include "GameViewPort.h"
#include "Engine/PostProcess/GreyScale.h"


extern World*		GWorld;
Camera				camera;

PostProcessRender   *greyscale_render;

//////////////////////////////////////////////////////////////////
// This function does any needed initialization on the rendering
// context. 
void SetupRC()
{
	GWorld = new World();
	GWorld->init();

// 	glGenTextures(1, &depthTextureID);
// 	glBindTexture(GL_TEXTURE_2D, depthTextureID);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
// 	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
// 	glBindTexture(GL_TEXTURE_2D, 0);

	greyscale_render = new GreyScale();
	greyscale_render->init();
}

void ShutdownRC(void)
{
	GWorld->destroy();
	delete GWorld;

	greyscale_render->destroy();
	delete greyscale_render;
}


///////////////////////////////////////////////////
// Screen changes size or is initialized
void ChangeSize(int nWidth, int nHeight)
{

	GWorld->onChangeSize(nWidth, nHeight);

	greyscale_render->onChangeSize(nWidth, nHeight);
}


// Called to draw scene
void RenderScene(void)
{
	if (!GWorld || !GWorld->getGameViewPort())
		return;

	int screenWidth = GWorld->getGameViewPort()->getWindowWidth(); 
	int screenHeight = GWorld->getGameViewPort()->getWindowHeight();

	// Clear the color and depth buffers
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);


	GWorld->draw();

	greyscale_render->render();

	// Do the buffer Swap
	glutSwapBuffers();

	// Tell GLUT to do it again
	glutPostRedisplay();
}


void KeyEventHandler(unsigned char key, int x, int y)
{
	camera.KeyEventHandler(key, x, y);
}

// Respond to arrow keys by moving the camera frame of reference
void SpecialKeyEventHandler(int key, int x, int y)
{
	camera.SpecialKeyEventHandler(key, x, y);
}

void MouseEventHandler(int button, int state, int x, int y)
{
	camera.MouseEventHandler(button, state, x, y);
}

void MouseMotionEventHandler(int x, int y)
{
	camera.MouseMotionEventHandler(x, y);
}

int main(int argc, char* argv[])
{
	gltSetWorkingDirectory(argv[0]);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 600);

	glutCreateWindow("OpenGL SphereWorld");

	glutKeyboardFunc(KeyEventHandler);
	glutSpecialFunc(SpecialKeyEventHandler);
	glutMotionFunc(MouseMotionEventHandler);
	glutMouseFunc(MouseEventHandler);
	glutReshapeFunc(ChangeSize);
	glutDisplayFunc(RenderScene);

	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
		return 1;
	}


	SetupRC();
	glutMainLoop();
	ShutdownRC();
	return 0;
}
