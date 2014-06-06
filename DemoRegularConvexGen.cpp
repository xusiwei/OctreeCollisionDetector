#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <stdio.h>

#include "DemoRegularConvexGen.h"

#ifdef TEST_REGULAER_CONVEX_GEN

int main(int argc, char **argv)
{
	GlutApp* app = new DemoRegularConvexGen();

	app->initGlut(argc, argv);
	app->setTitle("test Regular convex generator\n");
	app->setWindowsSize(600, 600);

	app->setTimer(1000, 100);

	app->run();

	return 0;
}

#endif // APP_TEST

void DemoRegularConvexGen::onKey( unsigned char key, int x, int y )
{
	bool depthChange = false;
	switch (key)
	{
	case '-':
		depth--;
		if (depth < 0)
		{
			depth = 0;
		}
		depthChange = true;
		break;
	case '+':
	case '=':
		depth++;
		depthChange = true;
		break;
	case ' ':
		printf("serialized to RegConvex%d.obj", depth);
		static char buff[64];
		sprintf(buff, "RegConvex%d.obj", depth);
		pConvex->serialize(buff);
	default:
		break;
	}

	if (depthChange)
	{
		// printf("depth: %d ", depth);
		Convex* pOld = pConvex;
		pConvex = generator->generate(depth);
		printf("depth: %d, vertices: %d, triangles: %d\n", depth,
			pConvex->getNumVertices(), pConvex->getNumTriangles());
		delete pOld;
	}
}

void DemoRegularConvexGen::onTimer()
{
	//static int count = 0;
	//printf("Alarm %d!\n", count++);
	angle += 1.0;
	glutPostRedisplay();
}

void DemoRegularConvexGen::onInit()
{
	angle = 0;
	depth = 1;
	printf("OnInit\n");

	generator = new RegularConvexGenerator();

	pConvex = generator->generate(depth);

	printf("number of vertices: %d\n", pConvex->getNumVertices());
	printf("number of triangles: %d\n", pConvex->getNumTriangles());

	//pConvex->serialize("convex1.obj");

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_SMOOTH);

	glEnable(GL_DEPTH_TEST);

	// glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_FRONT, GL_LINE);
	// glPolygonMode(GL_BACK, GL_FILL);
	//glCullFace(GL_BACK); // 剔除背面
}

void DemoRegularConvexGen::onResize( int w, int h )
{
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-2.0 * w / h, 2.0 * w / h, -2.0, 2.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); // 不能少
}

void DemoRegularConvexGen::onDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glRotated(angle, 0, 1, 0);
	glRotated(angle, 1, 1, 1);

	pConvex->render();

	//glutPostRedisplay();
}
