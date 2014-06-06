#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <stdio.h>
#include <time.h>

#include "common.h"
#include "DemoOctree.h"

static const float CAMERA_MOVE_SPEED = 0.05;
static const float CAMERA_ROTATE_SPEED = 0.01;

#ifdef TEST_DEMO_OCTREE

int main(int argc, char **argv)
{
	GlutApp* app = new DemoOctree();

	app->initGlut(argc, argv);
	app->setTitle("Octree collision detection demo");
	app->setWindowsSize(800, 600);

	app->run();

	return 0;
}

#endif // TEST_DEMO_OCTREE

void DemoOctree::onInit()
{
	octreeDisplayLevel = 0;
	printf("OnInit\n");

	// 创建 world.
	world = new cWorld();
	world->setBackgroundColor(0, 0, 0);

	// 创建 camera
	camera = new cCamera(world);
	world->addChild(camera);

	// 摆放相机
	camera->set( cVector3d (1.5, -6.0, 2.0),    // camera position (eye)
		cVector3d (1.5, 0.0, 0.0),    // lookat position (target)
		cVector3d (0.0, 1.0, 0.0));   // direction of the "up" vector

	camera->setClippingPlanes(0.01, 20.0);

	// 创建光源，并将光源添加到相机
	light = new cLight(world);
	camera->addChild(light);
	light->setEnabled(true);
	light->setPos(cVector3d(5, 5, 5));
	light->setDir(cVector3d(-5, -5, -5));

	// 创建 mesh1
	mesh1 = new cMesh(world);
	mesh1->setPos(0, 0, 0);
	world->addChild(mesh1); // 加入 world

	// 加载3d模型文件
	if ( ! mesh1->loadFromFile("duck.obj") )
	{
		printf("Error - 3D Model failed to load correctly.\n");
		exit(-1);
	}
	mesh1->setUseMaterial(true);
	mesh1->computeBoundaryBox(true);

	// 创建 mesh2
	mesh2 = new cMesh(world);
	mesh2->setPos(3, 0, 0);
	world->addChild(mesh2); // 加入 world

	// 加载3d模型文件
	if ( ! mesh2->loadFromFile("duck.obj") )
	{
		printf("Error - 3D Model failed to load correctly.\n");
		exit(-1);
	}
	// mesh2->rotate(cVector3d(0, 0, 1), 3.14);
	mesh2->setUseMaterial(true);
	mesh2->computeBoundaryBox(true);

	label = new cLabel();
	camera->m_front_2Dscene.addChild(label);
	label->setPos(0, 0, 0);

	// 创建 Octree
	tree1 = new Octree(mesh1, 0.05);
	tree1->setShowTree(true);
	tree1->build();
	tree2 = new Octree(mesh2, 0.05);
	tree2->setShowTree(true);
	tree2->build();
}

void DemoOctree::onDisplay()
{
	camera->renderView(getWindowWidth(), getWindowHeight());

	tree1->render();

	tree2->render();

	// Swap buffers
	glutSwapBuffers();
	glutPostRedisplay();

#if 1
	static int last = 0;
	if (last)
	{
		// printf("frame time: %d ms.\n", clock() - last);
		static char buff[256];
		sprintf(buff, "FPS: %3.1f", 1000 / (double)(clock() - last));
		label->m_string = string(buff);
	}
	last = clock();
#endif

	// check for any OpenGL errors
	GLenum err;
	err = glGetError();
	if (err != GL_NO_ERROR) printf("Error:  %s\n", gluErrorString(err));
}

void DemoOctree::onResize( int w, int h )
{
	//printf("resize window: %d, %d\n", w, h);
	glViewport(0, 0, w, h);
}

void DemoOctree::onKey( unsigned char key, int x, int y )
{
	cVector3d delta(0, 0, 0);
	// printf("key %c(%d) %d, %d\n", key, key, x, y);
	switch(key) {
	case 27:
	case 'x':
		exit(0);
		break;
	case '2':
		{
			bool useWireMode = mesh1->getWireMode();
			mesh1->setWireMode(!useWireMode, true);
			mesh2->setWireMode(!useWireMode, true);
		}
		break;
	case '3':
		{
			bool showTree = tree1->getShowTree();
			tree1->setShowTree(!showTree);
			tree2->setShowTree(!showTree);
		}
	case '-':
		{
			octreeDisplayLevel--;
			if (octreeDisplayLevel < 0) { octreeDisplayLevel = 0; }

			tree1->setDisplayDepth(octreeDisplayLevel);
			tree2->setDisplayDepth(octreeDisplayLevel);
		}
		break;
	case '+':
	case '=':
		{
			octreeDisplayLevel++;
			cColorf tmpColor = cColorf(1.0, 0.0, 0.0);

			tree1->setDisplayDepth(octreeDisplayLevel);
			tree2->setDisplayDepth(octreeDisplayLevel);
		}
		break;

		//////////////////////////////////////////////////////////////////////////
		// ASDW to move camera
	case 'w':
		delta = camera->getLookVector() * -CAMERA_MOVE_SPEED;
		camera->translate(delta);
		break;
	case 's':
		delta = camera->getLookVector() * CAMERA_MOVE_SPEED;
		camera->translate(delta);
		break;
	case 'd':
		delta = camera->getRightVector() * CAMERA_MOVE_SPEED;
		camera->translate(delta);
		break;
	case 'a':
		delta = camera->getRightVector() * -CAMERA_MOVE_SPEED;
		camera->translate(delta);
		break;
	case 'q':
		delta = camera->getUpVector() * CAMERA_MOVE_SPEED;
		camera->translate(delta);
		break;
	case 'e':
		delta = camera->getUpVector() * -CAMERA_MOVE_SPEED;
		camera->translate(delta);
		break;
	default:
		break;
	}
}

void DemoOctree::onSpecialKey( int key, int x, int y )
{
	cVector3d delta(0, 0, 0);

	switch (key)
	{
	case GLUT_KEY_UP:
		delta = camera->getUpVector() * CAMERA_MOVE_SPEED;
		break;
	case GLUT_KEY_DOWN:
		delta = camera->getUpVector() * -CAMERA_MOVE_SPEED;
		break;
	case GLUT_KEY_LEFT: // 'j':
		delta = camera->getRightVector() * -CAMERA_MOVE_SPEED;
		break;
	case GLUT_KEY_RIGHT: // 'l':
		delta = camera->getRightVector() * CAMERA_MOVE_SPEED;
		break;
	case GLUT_KEY_PAGE_UP: // 'i':
		delta = camera->getLookVector() * -CAMERA_MOVE_SPEED;
		break;
	case GLUT_KEY_PAGE_DOWN: // 'k':
		delta = camera->getLookVector() * CAMERA_MOVE_SPEED;
		break;
	default:
		break;
	}

	mesh1->translate(delta);
	tree1->translate(delta);

	bool collid = tree1->computeCollision(tree2, cdRecoder);
	if (collid)
	{
		printf("collision detected! %d\n", clock());
		printf("number of collisions detected: %d\n", cdRecoder.m_collisions.size());
		cdRecoder.m_collisions.clear();
	}
}
