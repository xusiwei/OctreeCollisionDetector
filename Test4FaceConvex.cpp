#include <stdio.h>
#include <windows.h>
#include <gl/GL.h>
#include <gl/glut.h>

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "common.h"
#include "GlutApp.h"
#include "Convex.h"

#ifdef TEST_4FACES

//#define  DOUBLE_FLOAT

// typedefs:
#ifdef DOUBLE_FLOAT
typedef glm::dvec3 Point;
typedef double     Float;
#else
typedef glm::vec3 Point;
typedef float     Float;
#endif

Float vertices[][3] = {
	0, 0, 0,
	1, 1, 0,
	0, 1, 1,
	1, 0, 1
};

#define NVERTICES sizeof(vertices)/sizeof(vertices[0])

int triangles[][3] = {
	0, 2, 1,
	0, 3, 2,
	0, 1, 3,
	1, 2, 3
};

#define NTRIANGLES sizeof(triangles)/sizeof(triangles[0])

Point vArray[NVERTICES];

Point colors[NVERTICES];

//Point rotateAxis(0, 1, 0);
Point rotateAxis(1, 1, 1);

Point yAxis(0, 1, 0);

Float angle = 1.0;


class Test4Face : public GlutApp
{
	Convex* pConvex;

	virtual void onTimer() 
	{
		//static int count = 0;
		//printf("Alarm %d!\n", count++);

		for (int i=0; i<NVERTICES; ++i)
		{
			vArray[i] = glm::rotate(vArray[i], angle, rotateAxis);
		}
		glutPostRedisplay();
	}

	virtual void onInit() 
	{
		pConvex = new Convex();

		printf("OnInit\n");
		PRINT("%d\n", sizeof(Point));
		PRINT("%d\n", sizeof(yAxis[0]));
		PRINT("%d\n", sizeof(double));

		Point center;
		for (int i=0; i<NVERTICES; ++i)
		{
			vArray[i] = Point(vertices[i][0], vertices[i][1], vertices[i][2]);
			center += vArray[i];
		}
		center /= NVERTICES;

		for (int i=0; i<NVERTICES; ++i)
		{
			vArray[i] -= center;
			vArray[i] = glm::normalize(vArray[i]);
		}

		Float deg = glm::degrees(glm::acos(glm::dot(vArray[0], yAxis)));
		Point axis = glm::cross(vArray[0], yAxis);

		printf("deg: %g\n", deg);
		for (int i=0; i<NVERTICES; ++i)
		{
			vArray[i] = glm::rotate(vArray[i], deg, axis);
			colors[i] = vArray[i];
			pConvex->addVertex(vArray[i]);
			PRINT_VECTOR(vArray[i]);
		}

		for (int i=0; i<NTRIANGLES; ++i)
		{
			pConvex->addTriangle(glm::ivec3(triangles[i][0], triangles[i][1], triangles[i][2]));
		}
		pConvex->serialize("4face.obj");

		glClearColor(0.0, 0.0, 0.0, 0.0);
		 glEnable(GL_DEPTH_TEST);

		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // 更改多边形绘制形
		glPolygonMode(GL_BACK, GL_LINE);

		//glMatrixMode(GL_MODELVIEW);
		//glLoadIdentity(); // 不能少
		//gluLookAt(
		//	3, 4, 5,
		//	0, 0, 0,
		//	0, 1, 0
		//	);
	}

	virtual void onDisplay() 
	{
		glClear(GL_COLOR_BUFFER_BIT);
		 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBegin(GL_TRIANGLES);
		for (int i=0; i<NTRIANGLES; ++i)
		{
#ifdef DOUBLE_FLOAT
			glVertex3dv((GLdouble*)&vArray[ triangles[i][0] ]);
			glVertex3dv((GLdouble*)&vArray[ triangles[i][1] ]);
			glVertex3dv((GLdouble*)&vArray[ triangles[i][2] ]);
#else
			glColor3fv((GLfloat*)&colors[  triangles[i][0] ]);
			glVertex3fv((GLfloat*)&vArray[ triangles[i][0] ]);

			glColor3fv((GLfloat*)&colors[  triangles[i][1] ]);
			glVertex3fv((GLfloat*)&vArray[ triangles[i][1] ]);

			glColor3fv((GLfloat*)&colors[  triangles[i][2] ]);
			glVertex3fv((GLfloat*)&vArray[ triangles[i][2] ]);
#endif
		}
		glEnd();

		GLenum err = glGetError();
		if (err != GL_NO_ERROR)
		{
			printf("onDisplay() ERROR: %s\n", gluErrorString(err));
		}

		glFlush();
		glutSwapBuffers();
	}

	void onResize(int w, int h)
	{
		glViewport(0, 0, w, h);

		//glMatrixMode(GL_PROJECTION);
		//glLoadIdentity();
		//gluOrtho2D(-2.0, 2.0, -2.0, 2.0);

		GLenum err = glGetError();
		if (err != GL_NO_ERROR)
		{
			printf("onResize() ERROR: %s\n", gluErrorString(err));
		}

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity(); // 不能少
	}
};

void test();

int main(int argc, char** argv)
{
	GlutApp* app = new Test4Face();

	app->initGlut(argc, argv);
	app->setTitle("Timer test");
	app->setWindowsSize(640, 480);

	app->setTimer(1000, 100);

	app->run();

	return 0;
}

void test()
{
	typedef glm::vec3 Point;

	Point O(0, 0, 0);

	Point A(0, 0, 0);
	Point B(1, 1, 0);
	Point C(0, 1, 1);
	Point D(1, 0, 1);

	Point G = (A+B+C+D) /= 4;

	printf("G:(%g, %g, %g)\n", G.x, G.y, G.z);

	printf("GA: %g\n", glm::distance(G, A));
	printf("GB: %g\n", glm::distance(G, B));
	printf("GC: %g\n", glm::distance(G, C));
	printf("GD: %g\n", glm::distance(G, D));

	Point aa = A - G;
	Point bb = B - G;
	Point cc = C - G;
	Point dd = D - G;

	PRINT_VECTOR(aa);
	PRINT_VECTOR(bb);
	PRINT_VECTOR(cc);
	PRINT_VECTOR(dd);

	A -= G;
	B -= G;
	C -= G;
	D -= G;

	A = glm::normalize(A);
	B = glm::normalize(B);
	C = glm::normalize(C);
	D = glm::normalize(D);

	PRINT_VECTOR(A);
	PRINT_VECTOR(B);
	PRINT_VECTOR(C);
	PRINT_VECTOR(D);
}


#endif // TEST_4FACES



