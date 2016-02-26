#include "GameViewPort.h"

void GameViewPort::onChangeSize(int nWidth, int nHeight)
{
	window_width = nWidth;
	window_height = nHeight;
	glViewport(0, 0, nWidth, nHeight);

	// Create the projection matrix, and load it on the projection matrix stack
	viewFrustum.SetPerspective(35.0f, float(nWidth) / float(nHeight), 1.0f, 100.0f);
}

const M3DMatrix44f& GameViewPort::GetProjectionMatrix(void)
{
	return viewFrustum.GetProjectionMatrix();
}

int GameViewPort::getWindowWidth()
{
	return window_width;
}

int GameViewPort::getWindowHeight()
{
	return window_height; 
}

GameViewPort::GameViewPort()
{

}
