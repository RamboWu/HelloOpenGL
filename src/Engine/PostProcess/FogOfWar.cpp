#include <math3d.h>
#include "../Core/World.h"
#include "../Core/Camera.h"
#include "../core/GameViewPort.h"
#include "FogOfWar.h"
#include "../../Util/Util.h"

extern World*		GWorld;
extern Camera				camera;

PostProcessRender* FogOfWar::init()
{
	if (!GWorld || !GWorld->getGameViewPort())
		return this;

	int window_width = GWorld->getGameViewPort()->getWindowWidth();
	int window_height = GWorld->getGameViewPort()->getWindowHeight();

	//����һ����ͶӰ
	gltGenerateOrtho2DMat(window_width, window_height, orthoMatrix, screenQuad);

	//׼�����ػ�����
	pixelDataSize = window_width*window_height * 3 * sizeof(unsigned int); // XXX This should be unsigned byte
	// Alloc space for copying pixels so we dont call malloc on every draw
	glGenBuffers(1, pixBuffObjs);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, pixBuffObjs[0]);
	glBufferData(GL_PIXEL_PACK_BUFFER, pixelDataSize, NULL, GL_DYNAMIC_COPY);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);



	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	myTexturedIdentityShader = gltLoadShaderPairWithAttributes("../../src/Shader/FogOfWar.vs", "../../src/Shader/FogOfWar.fs", 2,
		GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_TEXTURE0, "vTexCoords");

	glGenTextures(1, &depthTextureID);
	glBindTexture(GL_TEXTURE_2D, depthTextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 800, 600, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	return this;
}

void FogOfWar::destroy()
{
	// delete PBO
	glDeleteBuffers(1, pixBuffObjs);

	glDeleteTextures(1, &textureID);

	glDeleteProgram(myTexturedIdentityShader);
}

void FogOfWar::onChangeSize(int nWidth, int nHeight)
{
	//����һ����ͶӰ
	gltGenerateOrtho2DMat(nWidth, nHeight, orthoMatrix, screenQuad);

	//׼�����ػ�����
	pixelDataSize = nWidth*nHeight * 3 * sizeof(unsigned int); // XXX This should be unsigned byte

	glBindBuffer(GL_PIXEL_PACK_BUFFER, pixBuffObjs[0]);
	glBufferData(GL_PIXEL_PACK_BUFFER, pixelDataSize, NULL, GL_DYNAMIC_COPY);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
}

void FogOfWar::render()
{
	if (!GWorld || !GWorld->getGameViewPort())
		return;

	int window_width = GWorld->getGameViewPort()->getWindowWidth();
	int window_height = GWorld->getGameViewPort()->getWindowHeight();

	//��ȡ�������
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthTextureID);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 0, 0, window_width, window_height, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	
	//��ȡ��Ļ��ɫ����
	glBindBuffer(GL_PIXEL_PACK_BUFFER, pixBuffObjs[0]);
	glReadPixels(0, 0, window_width, window_height, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
	// Next bind the PBO as the unpack buffer, then push the pixels straight into the texture
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pixBuffObjs[0]);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);




	glDisable(GL_DEPTH_TEST);

	glUseProgram(myTexturedIdentityShader);
	GLint iMvpUniform = glGetUniformLocation(myTexturedIdentityShader, "mvpMatrix");
	glUniformMatrix4fv(iMvpUniform, 1, GL_FALSE, orthoMatrix);
	//����screenToWorld�ľ�����ʵ����ViewProjection��ת�þ���
	GLint iScreenToWorldUniform = glGetUniformLocation(myTexturedIdentityShader, "screenToWorldMatrix");
	M3DMatrix44f tmp1,tmp2,tmp3;
	camera.GetCameraMatrix(tmp1);
	m3dMatrixMultiply44(tmp2, GWorld->getGameViewPort()->GetProjectionMatrix(), tmp1);
	m3dInvertMatrix44(tmp3, tmp2);
	glUniformMatrix4fv(iScreenToWorldUniform, 1, GL_FALSE, tmp3);
	GLint iTextureUniform = glGetUniformLocation(myTexturedIdentityShader, "colorMap");
	glUniform1i(iTextureUniform, 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthTextureID);
	glUniform1i(glGetUniformLocation(myTexturedIdentityShader, "depthTexture"), 0);
	screenQuad.Draw();

	glEnable(GL_DEPTH_TEST);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);
}