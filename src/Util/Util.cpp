#include "Util.h"
#include <windows.h>


void Util::printMaxtrix44f(const M3DMatrix44f matrix)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			fprintf(stderr, "%.4f ", matrix[i * 4 + j]);

		}
		fprintf(stderr, "\n");
	}
	fprintf(stderr, "\n");
}

bool Util::LoadTGATexture(const char *szFileName, GLenum minFilter, GLenum magFilter, GLenum wrapMode)
{
	fprintf(stderr, "LoadTGATexture>> %s\n", szFileName);

	GLbyte *pBits;
	int nWidth, nHeight, nComponents;
	GLenum eFormat;

	fprintf(stderr, "%s \n", SclGetAppCurDir());
	
	// Read the texture bits
	pBits = gltReadTGABits(szFileName, &nWidth, &nHeight, &nComponents, &eFormat);
	if (pBits == NULL)
		return false;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, nComponents, nWidth, nHeight, 0,
		eFormat, GL_UNSIGNED_BYTE, pBits);

	free(pBits);

	if (minFilter == GL_LINEAR_MIPMAP_LINEAR ||
		minFilter == GL_LINEAR_MIPMAP_NEAREST ||
		minFilter == GL_NEAREST_MIPMAP_LINEAR ||
		minFilter == GL_NEAREST_MIPMAP_NEAREST)
		glGenerateMipmap(GL_TEXTURE_2D);

	return true;
}

LPCSTR Util::SclGetAppCurDir()
{
	static char szAppPath[MAX_PATH] = { '\0' };
	if (szAppPath[0] == '\0')
	{
		int i;
		GetModuleFileNameA(GetModuleHandle(NULL), szAppPath, MAX_PATH);
		for (i = (int)strlen(szAppPath) - 1; i > 0; --i)
		{
			if (szAppPath[i] == '\\')
			{
				break;
			}
		}

		szAppPath[i + 1] = '\0';
	}
	return szAppPath;
}

void Util::unprojectScreenToWorld(int x, int y, const M3DMatrix44f projection, const M3DMatrix44f view, int width, int height)
{
	float winZ;
	glReadPixels(
		int(x),//x坐标
		int(y),//y坐标
		1, 1,//读取一个像素
		GL_DEPTH_COMPONENT,//获得深度信息
		GL_FLOAT,//数据类型为浮点型
		&winZ);//获得的深度值保存在winZ中

	fprintf(stderr, "%.2f\n", winZ);

}