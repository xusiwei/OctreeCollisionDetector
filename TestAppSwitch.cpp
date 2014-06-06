#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <stdio.h>

#include "common.h"
#include "GlutApp.h"

#ifdef TEST_APP_SWITCH

GlutApp* app1;

GlutApp* app2;

class TestAppSwitch : public GlutApp
{
	virtual void onKeyDown( unsigned char key, int x, int y ) 
	{
		printf("app1 onKeyDown: %d(%c), <%d, %d>\n", key, key, x, y);
		switch (key)
		{
		case ' ':
			app2->run();
			break;
		default:
			break;
		}
	}

	virtual void onInit() 
	{
		printf("OnInit\n");

		glClearColor(0.0, 0.0, 0.0, 0.0);
		glShadeModel(GL_SMOOTH);
	}

	virtual void onDisplay() 
	{
		glClear(GL_COLOR_BUFFER_BIT);

		// glPolygonMode(GL_FRONT, GL_LINE);

		glBegin(GL_TRIANGLES);
			glColor3f(1, 0, 0);
			glVertex2f(-1, -1);

			glColor3f(0, 1, 0);
			glVertex2f(1,  -1);

			glColor3f(0, 0, 1);
			glVertex2f(0, 1);
		glEnd();

		glFlush();
		glutSwapBuffers();
	}
};

class TestAppAnother : public GlutApp
{
	virtual void onKeyDown( unsigned char key, int x, int y ) 
	{
		printf("app2 onKeyDown: %d(%c), <%d, %d>\n", key, key, x, y);
		switch (key)
		{
		case ' ':
			app1->run();
			break;
		default:
			break;
		}
	}

	virtual void onInit() 
	{
		printf("OnInit\n");

		glClearColor(0.0, 0.0, 0.0, 0.0);
		glShadeModel(GL_SMOOTH);
	}

	virtual void onDisplay() 
	{
		glClear(GL_COLOR_BUFFER_BIT);

		// glPolygonMode(GL_FRONT, GL_LINE);

		glBegin(GL_POLYGON);
			glColor3f(1, 0, 0);
			glVertex2f(-1, -1);

			glColor3f(0, 1, 0);
			glVertex2f(1,  -1);

			glColor3f(0, 0, 1);
			glVertex2f(1, 1);

			glColor3f(1, 1, 1);
			glVertex2f(-1, 1);
		glEnd();

		glFlush();
		glutSwapBuffers();
	}
};

int main(int argc, char **argv)
{
	app1 = new TestAppSwitch();
	app1->initGlut(argc, argv);
	app1->setTitle("app1");
	app1->setWindowsSize(640, 480);

	app2 = new TestAppAnother();
	app2->setTitle("app2");
	app2->setWindowsPos(0, 0);
	app2->setWindowsSize(800, 600);

	app1->run();

	return 0;
}

#endif // APP_TEST