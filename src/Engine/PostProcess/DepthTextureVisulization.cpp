#include "../Core/World.h"
#include "../core/GameViewPort.h"
#include "DepthTextureVisulization.h"
#include "../../Util/Util.h"

extern World*		GWorld;

PostProcessRender* DepthTextureVisulization::init()
{
	if (!GWorld || !GWorld->getGameViewPort())
		return this;

	int window_width = GWorld->getGameViewPort()->getWindowWidth();
	int window_height = GWorld->getGameViewPort()->getWindowHeight();

	//创建一个正投影
	gltGenerateOrtho2DMat(window_width, window_height, orthoMatrix, 0, 0, window_width, window_height, screenQuad);

	//准备像素缓冲区
	pixelDataSize = window_width*window_height * 3 * sizeof(unsigned int); // XXX This should be unsigned byte
	// Alloc space for copying pixels so we dont call malloc on every draw
	glGenBuffers(1, pixBuffObjs);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, pixBuffObjs[0]);
	glBufferData(GL_PIXEL_PACK_BUFFER, pixelDataSize, NULL, GL_DYNAMIC_COPY);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);



	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	if (Util::LoadTGATexture("stone.tga", GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE))
	{
		fprintf(stderr, "LoadTGATexture stone.tga success");
	}
	else
	{
		fprintf(stderr, "LoadTGATexture stone.tga failed");
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	myTexturedIdentityShader = gltLoadShaderPairWithAttributes("DepthTexture.vs", "DepthTexture.fs", 2,
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

void DepthTextureVisulization::destroy()
{
	// delete PBO
	glDeleteBuffers(1, pixBuffObjs);

	glDeleteTextures(1, &textureID);

	glDeleteProgram(myTexturedIdentityShader);
}

void DepthTextureVisulization::onChangeSize(int nWidth, int nHeight)
{
	//创建一个正投影
	gltGenerateOrtho2DMat(nWidth, nHeight, orthoMatrix, 0, 0, nWidth/3, nHeight/3, screenQuad);

	//准备像素缓冲区
	pixelDataSize = nWidth*nHeight * 3 * sizeof(unsigned int); // XXX This should be unsigned byte

	glBindBuffer(GL_PIXEL_PACK_BUFFER, pixBuffObjs[0]);
	glBufferData(GL_PIXEL_PACK_BUFFER, pixelDataSize, NULL, GL_DYNAMIC_COPY);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
}

void DepthTextureVisulization::render()
{
	if (!GWorld || !GWorld->getGameViewPort())
		return;

	int window_width = GWorld->getGameViewPort()->getWindowWidth();
	int window_height = GWorld->getGameViewPort()->getWindowHeight();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthTextureID);
	// Grab the screen pixels and copy into local memory
	//glReadPixels(0, 0, screenWidth, screenHeight, GL_DEPTH_COMPONENT, GL_FLOAT, pixelData);

	// Push pixels from client memory into texture
	// Setup texture unit for new blur, this gets imcremented every frame
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, pixelData);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 0, 0, window_width, window_height, 0);
	glBindTexture(GL_TEXTURE_2D, 0);


	// 将数据从 GPU的内存 放到 缓存中
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
	GLint iTextureUniform = glGetUniformLocation(myTexturedIdentityShader, "colorMap");
	glUniform1i(iTextureUniform, 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthTextureID);
	glUniform1i(glGetUniformLocation(myTexturedIdentityShader, "depthTexture"), 0);
	//shaderManager.UseStockShader(GLT_SHADER_TEXTURE_REPLACE, transformPipeline.GetModelViewProjectionMatrix(), 0);
	screenQuad.Draw();

	glEnable(GL_DEPTH_TEST);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);
}