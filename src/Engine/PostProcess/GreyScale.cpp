#include "../Core/World.h"
#include "../core/GameViewPort.h"
#include "GreyScale.h"
#include "../../Util/Util.h"

extern World*		GWorld;

PostProcessRender* GreyScale::init()
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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	myTexturedIdentityShader = gltLoadShaderPairWithAttributes("../../src/Shader/GrayScale.vs", "../../src/Shader/GrayScale.fs", 2,
		GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_TEXTURE0, "vTexCoords");

	return this;
}

void GreyScale::destroy()
{
	// delete PBO
	glDeleteBuffers(1, pixBuffObjs);

	glDeleteTextures(1, &textureID);

	glDeleteProgram(myTexturedIdentityShader);
}

void GreyScale::onChangeSize(int nWidth, int nHeight)
{
	//创建一个正投影
	gltGenerateOrtho2DMat(nWidth, nHeight, orthoMatrix, 0, 0, nWidth/3, nHeight/3, screenQuad);

	//准备像素缓冲区
	pixelDataSize = nWidth*nHeight * 3 * sizeof(unsigned int); // XXX This should be unsigned byte

	glBindBuffer(GL_PIXEL_PACK_BUFFER, pixBuffObjs[0]);
	glBufferData(GL_PIXEL_PACK_BUFFER, pixelDataSize, NULL, GL_DYNAMIC_COPY);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
}

void GreyScale::render()
{
	if (!GWorld || !GWorld->getGameViewPort())
		return;

	int window_width = GWorld->getGameViewPort()->getWindowWidth();
	int window_height = GWorld->getGameViewPort()->getWindowHeight();

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

	//shaderManager.UseStockShader(GLT_SHADER_TEXTURE_REPLACE, transformPipeline.GetModelViewProjectionMatrix(), 0);
	screenQuad.Draw();

	glEnable(GL_DEPTH_TEST);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);
}