#include <stdio.h>
#include <windows.h>
#include <gl/GL.h>
#include <gl/glut.h>

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "common.h"
#include "GlutApp.h"

#ifdef TEST_TIMER

#define USE_DEPTH

float vertices [][3] = {
	0, 0, 0, // 0
	0, 0, 1, // 1
	0, 1, 0, // 2
	0, 1, 1, // 3

	1, 0, 0, // 4
	1, 0, 1, // 5
	1, 1, 0, // 6
	1, 1, 1  // 7
};

int triangles[][3] = {
	// left
	0, 1, 3, 
	3, 2, 0,

	// right
	4, 6, 7, 
	7, 5, 4,

	// front
	1, 5, 7,
	7, 3, 1,

	// back
	0, 2, 6,
	6, 4, 0,

	// top
	2, 3, 7,
	7, 6, 2,

	// bottom
	0, 4, 5,
	5, 1, 0
};

int edges[][2] = {
	0, 1, // 0
	1, 3, // 1
	3, 2, // 2
	2, 0, // 3

	0, 4, // 4
	1, 5, // 5
	3, 7, // 6
	2, 6, // 7

	4, 5, // 8
	5, 7, // 9
	7, 6, // 10
	6, 4  // 11
};

int verticals[][2] = {
	0, 1,
	1, 2,
	2, 3,
	3, 0,
};

#define NTRIANGLES sizeof(triangles)/sizeof(triangles[0])

#define NVERTICES sizeof(vertices)/sizeof(vertices[0])

glm::vec3 srcArray[NVERTICES];

glm::vec3 colorArray[NVERTICES];

glm::vec3 color3[3] = {
	glm::vec3(1, 0, 0),
	glm::vec3(0, 1, 0),
	glm::vec3(0, 0, 1)
};

glm::vec3 dstArray[NVERTICES];

float angle = 1.0;

glm::vec3 rotateAxis(1, 1, 1);

void validateEdgeLength()
{
	for (int i=0; i<ARRAY_SIZE(edges); ++i)
	{
		double dist = glm::distance(dstArray[edges[i][0]], dstArray[edges[i][1]]);
		if (fabs(dist - 1.0) > .01)
		{
			printf("edge %d length delta rate over 5%\n", i);
		}
		printf("%g ", dist);
	}
	printf("\n");
}

class TimerTest : public GlutApp
{
	bool pause;

	void onKey(unsigned char key, int x, int y)
	{
		switch (key)
		{
		case ' ':
			pause = !pause;
			break;
		default:
			break;
		}
	}

	virtual void onTimer() 
	{
		if (!pause)
		{
			static int count = 0;
			printf("Alarm %d!\n", count++);

			angle += 1.0;
			for (int i=0; i<NVERTICES; ++i)
			{
				dstArray[i] = glm::rotate(srcArray[i], angle, rotateAxis);
				//printf("v %g %g %g\n", v3Array[i].x, v3Array[i].y, v3Array[i].z);
			}

			// validate...
			validateEdgeLength();

			//glutPostRedisplay();
		}
	}

	virtual void onInit() 
	{
		pause = false;
		printf("OnInit\n");
		glClearColor(0.0, 0.0, 0.0, 0.0);
#ifdef USE_DEPTH
		glEnable(GL_DEPTH_TEST);
#endif

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity(); // 不能少
		gluLookAt(
			3, 4, 5,
			0, 0, 0,
			0, 1, 0
			);

		for (int i=0; i<NVERTICES; ++i)
		{
			srcArray[i] = glm::vec3(vertices[i][0], vertices[i][1], vertices[i][2]);
			colorArray[i] = srcArray[i];
		}

		glPolygonMode(GL_BACK, GL_LINE); // 更改多边形绘制形
		glPolygonMode(GL_FRONT, GL_FILL);
	}

	virtual void onDisplay() 
	{
#ifdef USE_DEPTH
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#else
		glClear(GL_COLOR_BUFFER_BIT);
#endif

		glBegin(GL_TRIANGLES);
		for (int i=0; i<NTRIANGLES; ++i)
		{
			//glColor3fv((GLfloat*)&colorArray[ triangles[i][0] ]);
			glColor3fv((GLfloat*)&color3[0]);
			glVertex3fv((GLfloat*)&dstArray[ triangles[i][0] ]);
			
			// glColor3fv((GLfloat*)&colorArray[ triangles[i][1] ]);
			glColor3fv((GLfloat*)&color3[1]);
			glVertex3fv((GLfloat*)&dstArray[ triangles[i][1] ]);
			
			// glColor3fv((GLfloat*)&colorArray[ triangles[i][2] ]);
			glColor3fv((GLfloat*)&color3[2]);
			glVertex3fv((GLfloat*)&dstArray[ triangles[i][2] ]);
		}
		glEnd();
		
		GLenum err = glGetError();
		if (err != GL_NO_ERROR)
		{
			printf("ERROR: %s\n", gluErrorString(err));
		}

		glFlush();
		glutSwapBuffers();
		glutPostRedisplay();
	}

	void onResize(int w, int h)
	{
		glViewport(0, 0, w, h);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(-2.0, 2.0, -2.0, 2.0);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity(); // 不能少
	}
};

int main(int argc, char** argv)
{
	GlutApp* app = new TimerTest();

	app->initGlut(argc, argv);
	app->setTitle("Timer test");
	app->setWindowsSize(640, 480);

	app->setTimer(1000, 100);

	app->run();

	return 0;
}


#endif // TIMER_TEST



