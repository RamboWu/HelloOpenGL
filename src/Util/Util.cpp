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

	float dc_x = float(x) / width * 2 - 1;
	float dc_y = float(y) / height * 2 - 1;
	float dc_z = 1 - winZ * 2;

 	GLint    viewport[4];
	M3DMatrix44d view_m, projection_m;
	copy44fTo44d(view_m, view);
	copy44fTo44d(projection_m, projection);
 	GLdouble object_x, object_y, object_z;
 	glGetIntegerv(GL_VIEWPORT, viewport);
	fprintf(stderr, "%d %d %d %d\n", viewport[0], viewport[1], viewport[2], viewport[3]);
	gluUnProject((GLdouble)x, (GLdouble)y, (GLdouble)winZ, view_m, projection_m, viewport, &object_x, &object_y, &object_z);
 	
	fprintf(stderr, "screen[%d,%d] depth = %.2f\n", x, y, winZ);
	fprintf(stderr, "object[%.2f,%.2f,%.2f]\n", object_x, object_y, object_z);

	M3DMatrix44f tmp1, tmp2;
	m3dMatrixMultiply44(tmp1, projection, view);
	m3dInvertMatrix44(tmp2, tmp1);

	M3DVector4f world_pos;
	M3DVector4f dc;
	dc[0] = dc_x;
	dc[1] = dc_y;
	dc[2] = dc_z;
	dc[3] = 1;

	matrixMultiplyVector4f(world_pos, tmp2, dc);
	fprintf(stderr, "dc[%.2f,%.2f,%.2f,%.2f]\n", dc[0], dc[1], dc[2], dc[3]);
	fprintf(stderr, "world[%.2f,%.2f,%.2f,%.2f]\n", world_pos[0] / world_pos[3], world_pos[1] / world_pos[3], world_pos[2] / world_pos[3], 1.0f);
}

void Util::matrixMultiplyVector4f(M3DVector4f vOut, const M3DMatrix44f m, const M3DVector4f v)
{
	vOut[0] = m[0] * v[0] + m[1] * v[1] + m[2] * v[2] + m[3] * v[3];
	vOut[1] = m[4] * v[0] + m[5] * v[1] + m[6] * v[2] + m[7] * v[3];
	vOut[2] = m[8] * v[0] + m[9] * v[1] + m[10] * v[2] + m[11] * v[3];
	vOut[3] = m[12] * v[0] + m[13] * v[1] + m[14] * v[2] + m[15] * v[3];
}

void Util::copy44fTo44d(M3DMatrix44d vOut, const M3DMatrix44f m)
{
	for (int i = 0; i < 16; i++)
	{
		vOut[i] = GLdouble(m[i]);
	}
}