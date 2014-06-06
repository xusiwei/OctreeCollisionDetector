#include <GL/glut.h>

#include "GlutStuff.h"
#include "GlutApp.h"

static GlutApp* gDemoApplication = 0;

static	void KeyboardCallback(unsigned char key, int x, int y)
{
	gDemoApplication->onKey(key,x,y);
}

static	void KeyboardUpCallback(unsigned char key, int x, int y)
{
	gDemoApplication->onKeyDown(key,x,y);
}

static void SpecialKeyboardCallback(int key, int x, int y)
{
	gDemoApplication->onSpecialKey(key,x,y);
}

static void SpecialKeyboardUpCallback(int key, int x, int y)
{
	gDemoApplication->onSpecialKeyDown(key,x,y);
}


static void ReshapeCallback(int w, int h)
{
	gDemoApplication->onResize(w,h);
}

static void IdleCallback()
{
	gDemoApplication->onIdle();
}

static void MouseFuncCallback(int button, int state, int x, int y)
{
	gDemoApplication->onMousePress(button,state,x,y);
}


static void	MotionFuncCallback(int x,int y)
{
	gDemoApplication->onMousePressMove(x,y);
}

static void MousePassiveCallback(int x, int y)
{
	gDemoApplication->onMouseMove(x, y);
}

static void DisplayCallback(void)
{
	gDemoApplication->onDisplay();
}


int glutmain(int argc, char **argv,int width,int height,const char* title, GlutApp* demoApp) {

	gDemoApplication = demoApp;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	// glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
	
	int screenW = glutGet(GLUT_SCREEN_WIDTH);
	int screenH = glutGet(GLUT_SCREEN_HEIGHT);
	int windowPosX = (screenW - width) / 2;
	int windowPosY = (screenH - height) / 2;

	glutInitWindowPosition(windowPosX, windowPosY);
	glutInitWindowSize(width, height);
	glutCreateWindow(title);

	gDemoApplication->onInit();

	// register keyboard callbacks
	glutKeyboardFunc(KeyboardCallback);
	glutKeyboardUpFunc(KeyboardUpCallback);
	glutSpecialFunc(SpecialKeyboardCallback);
	glutSpecialUpFunc(SpecialKeyboardUpCallback);
	
	// register mouse callbacks
	glutMouseFunc(MouseFuncCallback);
	glutMotionFunc(MotionFuncCallback);
	glutPassiveMotionFunc(MousePassiveCallback);

	glutReshapeFunc(ReshapeCallback);
	glutDisplayFunc(DisplayCallback);
	
	glutIdleFunc(IdleCallback);
	
	IdleCallback();

	glutMainLoop();
	return 0;
}

