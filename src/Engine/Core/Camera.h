#ifndef __CEMERA_H_
#define __CEMERA_H_

#include <GLFrame.h>
#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

class Camera: public GLFrame
{
public:

	void KeyEventHandler(unsigned char key, int x, int y);

	// Respond to arrow keys by moving the camera frame of reference
	void SpecialKeyEventHandler(int key, int x, int y);

	void MouseEventHandler(int button, int state, int x, int y);

	void MouseMotionEventHandler(int x, int y);

protected:
	int last_mouse_click_posx, last_mouse_click_posy;
};
#endif