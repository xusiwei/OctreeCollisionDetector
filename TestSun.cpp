#include <stdio.h>
#include <windows.h>
#include <gl/GL.h>
#include <gl/glut.h>

#include "common.h"

#include "GlutApp.h"

#ifdef TEST_SUN

static int year = 0, day = 0;

class TestSun : public GlutApp
{
	virtual void onTimer() 
	{
		//static int count = 0;
		//printf("Alarm %d!\n", count++);

		year = (year+1) % 360;
		glutPostRedisplay();
	}

	virtual void onInit() 
	{
		glClearColor(0.0, 0.0, 0.0, 0.0);
		glShadeModel(GL_SMOOTH);

		GLfloat mat_specular[] = { 1, 1, 1, 1 };
		GLfloat mat_shininess[] = {50};
		GLfloat light_position[] = {1, 1, 1, 0};
		// GLfloat light_position[] = {1, 1, 1, 1};
		GLfloat white_light[] = {1, 1, 1, 1};
		GLfloat model_ambint[] = {.5, .5, .5, 1};

		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);

		glLightfv(GL_LIGHT0, GL_POSITION, light_position);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
		glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);
		//glLightModelfv(GL_LIGHT_MODEL_AMBIENT, model_ambint);

		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glEnable(GL_DEPTH_TEST);

		// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // 更改多边形绘制形
	}

	virtual void onDisplay() 
	{
		GLfloat light_position[] = {0, 0, 0, 1};
		//glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glColor3f(1, 0, 0);
		glPushMatrix();
		glutSolidSphere(1.0, 20, 16); // draw sun.
		
		glRotatef((GLfloat)year, 0.0, 1.0, 0.0);
		glTranslatef(2.0, 0.0, 0.0);
		glRotatef((GLfloat) day, 0.0, 1.0, 0.0);

		// glLightfv(GL_LIGHT0, GL_POSITION, light_position);

		glColor3f(1.0, 1.0, 1.0);
		glutSolidSphere(0.2, 10, 8); // draw planet.
		glPopMatrix();

		GLenum err = glGetError();
		if (err != GL_NO_ERROR)
		{
			printf("onDisplay() ERROR: %s\n", gluErrorString(err));
		}
		glutSwapBuffers();
	}

	void onResize(int w, int h)
	{
		glViewport(0, 0, w, h);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(60.0, (GLfloat)w/(GLfloat)h, 1.0, 20.0);
		
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(
			0.0, 0.0, 5.0, 
			0.0, 0.0, 0.0,
			0.0, 1.0, 0.0
			);

		GLenum err = glGetError();
		if (err != GL_NO_ERROR)
		{
			printf("onResize() ERROR: %s\n", gluErrorString(err));
		}
	}

	void onKey(unsigned char key, int x, int y)
	{
		switch (key)
		{
		case 'd':
			day = (day+10) % 360;
			glutPostRedisplay();
			break;
		case 'D':
			day = (day-10) % 360;
			glutPostRedisplay();
			break;
		case 'y':
			year = (year+5) % 360;
			glutPostRedisplay();
			break;
		case 'Y':
			year = (year-5) % 360;
			glutPostRedisplay();
			break;
		default:
			break;
		}
	}
};

int main(int argc, char** argv)
{
	GlutApp* app = new TestSun();

	app->initGlut(argc, argv);
	app->setTitle("Timer test");
	app->setWindowsSize(640, 480);

	app->setTimer(1000, 20);

	app->run();

	return 0;
}

#endif // TEST_4FACES



