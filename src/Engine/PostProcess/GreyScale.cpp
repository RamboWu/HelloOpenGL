#include "GreyScale.h"
#include "../../Util/Util.h"

extern int window_width, window_height;

void GreyScale::init()
{
	//创建一个正投影
	gltGenerateOrtho2DMat(window_width, window_height, orthoMatrix, 0, 0, window_width / 2, window_height / 2, screenQuad);

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

	myTexturedIdentityShader = gltLoadShaderPairWithAttributes("TexturedIdentity.vs", "TexturedIdentity.fs", 2,
		GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_TEXTURE0, "vTexCoords");
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
	gltGenerateOrtho2DMat(window_width, window_height, orthoMatrix, 0, 0, window_width / 3, window_height / 3, screenQuad);

	//准备像素缓冲区
	pixelDataSize = window_width*window_height * 3 * sizeof(unsigned int); // XXX This should be unsigned byte

	glBindBuffer(GL_PIXEL_PACK_BUFFER, pixBuffObjs[0]);
	glBufferData(GL_PIXEL_PACK_BUFFER, pixelDataSize, NULL, GL_DYNAMIC_COPY);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
}

void GreyScale::render()
{
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