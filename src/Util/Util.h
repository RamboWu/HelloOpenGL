#ifndef __UTIL_H_
#define __UTIL_H_

#include <math3d.h>

class Util
{
public:

	static void printMaxtrix44f(const M3DMatrix44f matrix)
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				fprintf(stderr, "%.2f ", matrix[i * 4 + j]);

			}
			fprintf(stderr, "\n");
		}
	}
};
#endif