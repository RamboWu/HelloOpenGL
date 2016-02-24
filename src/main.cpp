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
#include <StopWatch.h>

#include <math.h>
#include <stdio.h>

#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

#include "Camera.h"

#define NUM_SPHERES 50
GLFrame spheres[NUM_SPHERES];


GLShaderManager		shaderManager;			// Shader Manager
GLMatrixStack		modelViewMatrix;		// Modelview Matrix
GLMatrixStack		projectionMatrix;		// Projection Matrix
GLFrustum			viewFrustum;			// View Frustum
GLGeometryTransform	transformPipeline;		// Geometry Transform Pipeline

GLTriangleBatch		torusBatch;
GLBatch				floorBatch;
GLTriangleBatch     sphereBatch;
Camera				camera;

int window_width, window_height;

GLuint				pixBuffObjs[1];
GLuint				pixelDataSize;
GLBatch             screenQuad;
M3DMatrix44f        orthoMatrix;

//////////////////////////////////////////////////////////////////
// This function does any needed initialization on the rendering
// context. 
void SetupRC()
{
	// Initialze Shader Manager
	shaderManager.InitializeStockShaders();

	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// This makes a torus
	gltMakeTorus(torusBatch, 0.4f, 0.15f, 30, 30);

	// This make a sphere
	gltMakeSphere(sphereBatch, 0.1f, 26, 13);

	floorBatch.Begin(GL_LINES, 324);
	for (GLfloat x = -20.0; x <= 20.0f; x += 0.5) {
		floorBatch.Vertex3f(x, -0.55f, 20.0f);
		floorBatch.Vertex3f(x, -0.55f, -20.0f);

		floorBatch.Vertex3f(20.0f, -0.55f, x);
		floorBatch.Vertex3f(-20.0f, -0.55f, x);
	}
	floorBatch.End();

	// Randomly place the spheres
	for (int i = 0; i < NUM_SPHERES; i++) {
		GLfloat x = ((GLfloat)((rand() % 400) - 200) * 0.1f);
		GLfloat z = ((GLfloat)((rand() % 400) - 200) * 0.1f);
		spheres[i].SetOrigin(x, 0.0f, z);
	}

	//创建一个正投影
	gltGenerateOrtho2DMat(window_width, window_height, orthoMatrix, 0, 0, window_width/2, window_height/2, screenQuad);

	//准备像素缓冲区
	pixelDataSize = window_width*window_height * 3 * sizeof(unsigned int); // XXX This should be unsigned byte

	// Alloc space for copying pixels so we dont call malloc on every draw
	glGenBuffers(1, pixBuffObjs);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, pixBuffObjs[0]);
	glBufferData(GL_PIXEL_PACK_BUFFER, pixelDataSize, NULL, GL_DYNAMIC_COPY);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
}

void ShutdownRC(void)
{
	// delete PBO
	glDeleteBuffers(1, pixBuffObjs);
}


///////////////////////////////////////////////////
// Screen changes size or is initialized
void ChangeSize(int nWidth, int nHeight)
{
	window_width = nWidth;
	window_height = nHeight;
	glViewport(0, 0, nWidth, nHeight);

	// Create the projection matrix, and load it on the projection matrix stack
	viewFrustum.SetPerspective(35.0f, float(nWidth) / float(nHeight), 1.0f, 100.0f);
	projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());

	// Set the transformation pipeline to use the two matrix stacks 
	transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);


	//创建一个正投影
	gltGenerateOrtho2DMat(window_width, window_height, orthoMatrix, 0, 0, window_width / 3, window_height / 3, screenQuad);

	//准备像素缓冲区
	pixelDataSize = window_width*window_height * 3 * sizeof(unsigned int); // XXX This should be unsigned byte

	glBindBuffer(GL_PIXEL_PACK_BUFFER, pixBuffObjs[0]);
	glBufferData(GL_PIXEL_PACK_BUFFER, pixelDataSize, NULL, GL_DYNAMIC_COPY);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
}


// Called to draw scene
void RenderScene(void)
{
	// Color values
	static GLfloat vFloorColor[] = { 0.0f, 1.0f, 0.0f, 1.0f };
	static GLfloat vTorusColor[] = { 1.0f, 0.0f, 0.0f, 1.0f };
	static GLfloat vSphereColor[] = { 0.0f, 0.0f, 1.0f, 1.0f };

	// Time Based animation
	static CStopWatch	rotTimer;
	float yRot = rotTimer.GetElapsedSeconds() * 60.0f;

	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	// Save the current modelview matrix (the identity matrix)
	modelViewMatrix.PushMatrix();

	M3DMatrix44f mCamera;
	camera.GetCameraMatrix(mCamera);
	modelViewMatrix.PushMatrix(mCamera);

	// Draw the ground
	shaderManager.UseStockShader(GLT_SHADER_FLAT,
		transformPipeline.GetModelViewProjectionMatrix(),
		vFloorColor);
	floorBatch.Draw();

	for (int i = 0; i < NUM_SPHERES; i++) {
		modelViewMatrix.PushMatrix();
		modelViewMatrix.MultMatrix(spheres[i]);
		shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(),
			vSphereColor);
		sphereBatch.Draw();
		modelViewMatrix.PopMatrix();
	}

	// Draw the spinning Torus
	modelViewMatrix.Translate(0.0f, 0.0f, -2.5f);

	// Save the Translation
	modelViewMatrix.PushMatrix();

	// Apply a rotation and draw the torus
	modelViewMatrix.Rotate(yRot, 0.0f, 1.0f, 0.0f);
	shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vTorusColor);
	torusBatch.Draw();
	modelViewMatrix.PopMatrix(); // "Erase" the Rotation from before

	// Apply another rotation, followed by a translation, then draw the sphere
	modelViewMatrix.Rotate(yRot * -2.0f, 0.0f, 1.0f, 0.0f);
	modelViewMatrix.Translate(0.8f, 0.0f, 0.0f);
	shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vSphereColor);
	sphereBatch.Draw();

	// Restore the previous modleview matrix (the identity matrix)
	modelViewMatrix.PopMatrix();
	modelViewMatrix.PopMatrix();


	glBindBuffer(GL_PIXEL_PACK_BUFFER, pixBuffObjs[0]);
	glReadPixels(0, 0, window_width, window_height, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);


	// Next bind the PBO as the unpack buffer, then push the pixels straight into the texture
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pixBuffObjs[0]);

	glActiveTexture(GL_TEXTURE0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);


	projectionMatrix.PushMatrix();
	projectionMatrix.LoadIdentity();
	projectionMatrix.LoadMatrix(orthoMatrix);
	modelViewMatrix.PushMatrix();
	modelViewMatrix.LoadIdentity();
	glDisable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vTorusColor);
	screenQuad.Draw();
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	modelViewMatrix.PopMatrix();
	projectionMatrix.PopMatrix();

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
