#include <GLTools.h>	// OpenGL toolkit
#include "Util.h"
//#include <windows.h>


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


#pragma pack(1)
typedef struct
{
    GLbyte	identsize;              // Size of ID field that follows header (0)
    GLbyte	colorMapType;           // 0 = None, 1 = paletted
    GLbyte	imageType;              // 0 = none, 1 = indexed, 2 = rgb, 3 = grey, +8=rle
    unsigned short	colorMapStart;          // First colour map entry
    unsigned short	colorMapLength;         // Number of colors
    unsigned char 	colorMapBits;   // bits per palette entry
    unsigned short	xstart;                 // image x origin
    unsigned short	ystart;                 // image y origin
    unsigned short	width;                  // width in pixels
    unsigned short	height;                 // height in pixels
    GLbyte	bits;                   // bits per pixel (8 16, 24, 32)
    GLbyte	descriptor;             // image descriptor
} TGAHEADER;
#pragma pack(8)

////////////////////////////////////////////////////////////////////
// Allocate memory and load targa bits. Returns pointer to new buffer,
// height, and width of texture, and the OpenGL format of data.
// Call free() on buffer when finished!
// This only works on pretty vanilla targas... 8, 24, or 32 bit color
// only, no palettes, no RLE encoding.
GLbyte * Util::ReadTGABits(const char *szFileName, GLint *iWidth, GLint *iHeight, GLint *iComponents, GLenum *eFormat)
{
    FILE *pFile;			// File pointer
    TGAHEADER tgaHeader;		// TGA file header
    unsigned long lImageSize;		// Size in bytes of image
    short sDepth;			// Pixel depth;
    GLbyte	*pBits = NULL;          // Pointer to bits
    
    // Default/Failed values
    *iWidth = 0;
    *iHeight = 0;
    *eFormat = GL_RGB;
    *iComponents = GL_RGB;
    
    // Attempt to open the file
    pFile = fopen(szFileName, "rb");
    if(pFile == NULL)
        return NULL;
    
    // Read in header (binary)
    fread(&tgaHeader, 18/* sizeof(TGAHEADER)*/, 1, pFile);
    
    // Do byte swap for big vs little endian
#ifdef __APPLE__
    LITTLE_ENDIAN_WORD(&tgaHeader.colorMapStart);
    LITTLE_ENDIAN_WORD(&tgaHeader.colorMapLength);
    LITTLE_ENDIAN_WORD(&tgaHeader.xstart);
    LITTLE_ENDIAN_WORD(&tgaHeader.ystart);
    LITTLE_ENDIAN_WORD(&tgaHeader.width);
    LITTLE_ENDIAN_WORD(&tgaHeader.height);
#endif
    
    
    // Get width, height, and depth of texture
    *iWidth = tgaHeader.width;
    *iHeight = tgaHeader.height;
    sDepth = tgaHeader.bits / 8;
    
    // Put some validity checks here. Very simply, I only understand
    // or care about 8, 24, or 32 bit targa's.
    if(tgaHeader.bits != 8 && tgaHeader.bits != 24 && tgaHeader.bits != 32)
        return NULL;
    
    // Calculate size of image buffer
    lImageSize = tgaHeader.width * tgaHeader.height * sDepth;
    
    // Allocate memory and check for success
    pBits = (GLbyte*)malloc(lImageSize * sizeof(GLbyte));
    if(pBits == NULL)
        return NULL;
    
    // Read in the bits
    // Check for read error. This should catch RLE or other
    // weird formats that I don't want to recognize
    if(fread(pBits, lImageSize, 1, pFile) != 1)
    {
        free(pBits);
        return NULL;
    }
    
    // Set OpenGL format expected
    switch(sDepth)
    {
#ifndef OPENGL_ES
        case 3:     // Most likely case
            *eFormat = GL_BGR;
            *iComponents = GL_RGB;
            break;
#endif
        case 4:
            *eFormat = GL_BGRA;
            *iComponents = GL_RGBA;
            break;
        case 1:
            *eFormat = GL_LUMINANCE;
            *iComponents = GL_LUMINANCE;
            break;
        default:        // RGB
            // If on the iPhone, TGA's are BGR, and the iPhone does not
            // support BGR without alpha, but it does support RGB,
            // so a simple swizzle of the red and blue bytes will suffice.
            // For faster iPhone loads however, save your TGA's with an Alpha!
#ifdef OPENGL_ES
            for(int i = 0; i < lImageSize; i+=3)
            {
                GLbyte temp = pBits[i];
                pBits[i] = pBits[i+2];
                pBits[i+2] = temp;
            }
#endif
            break;
    }
    
    
    
    // Done with File
    fclose(pFile);
    
    // Return pointer to image data
    return pBits;
}

bool Util::LoadTGATexture(const char *szFileName, GLenum minFilter, GLenum magFilter, GLenum wrapMode)
{
	fprintf(stderr, "LoadTGATexture>> %s\n", szFileName);

	GLbyte *pBits;
	int nWidth, nHeight, nComponents;
	GLenum eFormat;

	//fprintf(stderr, "%s \n", SclGetAppCurDir());
	
	// Read the texture bits
    pBits = Util::ReadTGABits(szFileName, &nWidth, &nHeight, &nComponents, &eFormat);
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

/*
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
}*/

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
	float dc_z = winZ * 2 - 1;

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

	m3dTransformVector4(world_pos, dc, tmp2);
	//matrixMultiplyVector4f(world_pos, tmp2, dc);
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