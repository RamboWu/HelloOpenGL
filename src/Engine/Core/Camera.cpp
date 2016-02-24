#include "Camera.h"

extern int window_width, window_height;

void Camera::KeyEventHandler(unsigned char key, int x, int y)
{
	float linear = 0.2f;

	//fprintf(stderr, "%d ", key);

	// W key
	if (key == 119)
		MoveForward(linear);
	// S key
	if (key == 115)
		MoveForward(-linear);
	// A key
	if (key == 97)
		MoveRight(linear);
	// D key
	if (key == 100)
		MoveRight(-linear);
	// Q key
	if (key == 113)
		TranslateWorld(0, -linear, 0);
	// E key
	if (key == 101)
		TranslateWorld(0, linear, 0);
}

// Respond to arrow keys by moving the camera frame of reference
void Camera::SpecialKeyEventHandler(int key, int x, int y)
{
	float linear = 0.1f;
	float angular = float(m3dDegToRad(5.0f));

	if (key == GLUT_KEY_UP)
		MoveForward(linear);

	if (key == GLUT_KEY_DOWN)
		MoveForward(-linear);

	if (key == GLUT_KEY_LEFT)
		RotateWorld(angular, 0.0f, 1.0f, 0.0f);

	if (key == GLUT_KEY_RIGHT)
		RotateWorld(-angular, 0.0f, 1.0f, 0.0f);
}

void Camera::MouseEventHandler(int button, int state, int x, int y)
{

	last_mouse_click_posx = x;
	last_mouse_click_posy = y;

}

void Camera::MouseMotionEventHandler(int x, int y)
{

	float x_degree = (float(last_mouse_click_posx - x)) / window_width * 90;
	float y_degree = (float(last_mouse_click_posy - y)) / window_height * 35;

	RotateWorld(float(m3dDegToRad(x_degree)), 0.0f, 1.0f, 0.0f);
	RotateLocal(float(m3dDegToRad(-y_degree)), 1.0f, 0.0f, 0.0f);

	last_mouse_click_posx = x;
	last_mouse_click_posy = y;

	glutPostRedisplay();
}