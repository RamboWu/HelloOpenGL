#include "World.h"
#include "GameViewPort.h"
#include "Camera.h"

#include <StopWatch.h>

World*	GWorld = NULL;

extern Camera				camera;

World::World()
{
	m_game_view_port = NULL;
}

void World::update()
{

}

void World::init()
{
	m_game_view_port = new GameViewPort();

	// Initialze Shader Manager
	shaderManager.InitializeStockShaders();

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
}

void World::draw()
{
	if (!m_game_view_port)
		return;

	// Color values
	static GLfloat vFloorColor[] = { 0.0f, 1.0f, 0.0f, 1.0f };
	static GLfloat vTorusColor[] = { 1.0f, 0.0f, 0.0f, 1.0f };
	static GLfloat vSphereColor[] = { 0.0f, 0.0f, 1.0f, 1.0f };

	// Time Based animation
	static CStopWatch	rotTimer;
	float yRot = rotTimer.GetElapsedSeconds() * 60.0f;

	GLMatrixStack		modelViewMatrix;
	GLMatrixStack		projectionMatrix;
	GLGeometryTransform	transformPipeline;
	modelViewMatrix.LoadIdentity();
	projectionMatrix.LoadMatrix(m_game_view_port->GetProjectionMatrix());
	transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);

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
}

void World::destroy()
{

}

void World::onChangeSize(int nWidth, int nHeight)
{
	if (m_game_view_port)
	{
		m_game_view_port->onChangeSize(nWidth, nHeight);
	}
	
}

GameViewPort* World::getGameViewPort()
{
	return m_game_view_port;
}