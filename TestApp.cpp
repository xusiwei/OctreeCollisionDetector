#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <stdio.h>

#include "common.h"
#include "GlutApp.h"

#ifdef TEST_APP

class TestApp : public GlutApp
{
	virtual void onSpecialKeyDown( int key, int x, int y ) 
	{
		printf("onKeyDown: %d(%c), <%d, %d>\n", key, key, x, y);
	}

	virtual void onSpecialKey( int key, int x, int y ) 
	{
		printf("onSpecialKey: %d(%c), <%d, %d>\n", key, key, x, y);
	}

	virtual void onKeyDown( unsigned char key, int x, int y ) 
	{
		printf("onKeyDown: %d(%c), <%d, %d>\n", key, key, x, y);
	}

	virtual void onKey( unsigned char key, int x, int y ) 
	{
		printf("onKey: %d(%c), <%d, %d>\n", key, key, x, y);
	}

	virtual void onMouseMove( int x, int y ) 
	{
		printf("onMouseMove: %d, %d\n", x, y);
	}

	virtual void onMousePress( int button, int state, int x, int y ) 
	{
		printf("onMousePress: %d, %d, %d, %d\n", button, state, x, y);
	}

	virtual void onMousePressMove( int x,int y ) 
	{
		printf("onMousePressMove: %d, %d\n", x, y);
	}

	virtual void onInit() 
	{
		printf("OnInit\n");

		glClearColor(0.0, 0.0, 0.0, 0.0);
		glShadeModel(GL_FLAT);
	}

	virtual void onDisplay() 
	{
		glClear(GL_COLOR_BUFFER_BIT);

		glPolygonMode(GL_FRONT, GL_LINE);

		glBegin(GL_TRIANGLES);
			glColor3f(1, 0, 0);
			glVertex2f(-1, -1);
			glVertex2f(1,  -1);
			glVertex2f(0, 1);
		glEnd();

		glFlush();
		glutSwapBuffers();
	}

	virtual void onResize( int w, int h ) 
	{
		printf("resize window: %d, %d\n", w, h);
	}

	virtual void onIdle() 
	{
		
	}
};

void menu1() { printf("menu1 selected!\n"); }

void menu2() { printf("menu2 selected!\n"); }

void fullScreen() { glutFullScreen(); }

void exitApp() { exit(0); }

int main(int argc, char **argv)
{
	TestApp test;

	test.initGlut(argc, argv);
	test.setTitle("AppTest");
	test.setWindowsSize(640, 480);
	test.setDisplayMode(GLUT_RGBA | GLUT_SINGLE);

	test.addRightMenu("menu1", menu1);
	test.addRightMenu("menu2", menu2);
	test.addRightMenu("full screen", fullScreen);
	test.addRightMenu("exit", exitApp);

	test.run();

	return 0;
}

#endif // APP_TEST