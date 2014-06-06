#include <windows.h>
#include <gl/GL.h>
#include <gl/glut.h>

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <stdio.h>
#include <memory>
#include <stack>

#include "DemoAabbAabbCd.h"

static const double MOVE_SPEED = 0.05;

static const double CAMERA_ROTATE_SPEED = 0.01;

static cColorf treeColor = cColorf(1, 0, 0);

#ifdef TEST_DEMO_AABB_AABB_CD

int main(int argc, char** argv)
{
	GlutApp* app = new DemoAabbAabbCd();

	app->initGlut(argc, argv);
	app->setTitle("AABB test");
	app->setWindowsSize(800, 600);

	app->run();

	return 0;
}

#endif // TEST_AABB




void DemoAabbAabbCd::onInit()
{
	lastX = 0;
	lastY = 0;
	collisionTreeDisplayLevel = 0;
	glEnable(GL_DEPTH_TEST);

	printf("OnInit\n");
	// 创建 world.
	world = new cWorld();
	world->setBackgroundColor(0, 0, 0);

	// 创建 camera
	camera = new cCamera(world);
	world->addChild(camera);

	// 摆放相机
	camera->set( 
		cVector3d (1.5, -5.0, 1.0), // camera position (eye)
		// cVector3d (1.5, 2, 5), // camera position (eye)
		cVector3d (1.5, 0.0, 0.0),  // lookat position (target)
		cVector3d (0.0, 1.0, 0.0)   // direction of the "up" vector
		);  

	camera->setClippingPlanes(0.01, 20.0);

	// 创建光源，并将光源添加到相机
	light = new cLight(world);
	camera->addChild(light);
	light->setEnabled(true);
	light->setPos(cVector3d(5, 5, 5));
	light->setDir(cVector3d(-5, -5, -5));

	// 创建 mesh
	object = new cMesh(world);
	object->setPos(0.0, 0.0, 0.0);
	world->addChild(object);  // mesh 加入 world

	if ( ! object->loadFromFile("duck.obj") ) // 4face suzanne
	{
		printf("Error - 3D Model failed to load correctly.\n");
		exit(-1);
	}
	object->setUseMaterial(true);
	object->computeBoundaryBox(true);

	// 添加 AABB 碰撞检测器
	object->createAABBCollisionDetector( 0.01, true, false);

#if 1
	obj2 = new cMesh(world);
	obj2->setPos(3, 0, 0);
	world->addChild(obj2);

	if ( ! obj2->loadFromFile("duck.obj")) 
	{
		printf("Error - 3D Model failed to load correctly.\n");
		exit(-1);
	}
	obj2->setUseMaterial(true);
	obj2->computeBoundaryBox(true);

	obj2->createAABBCollisionDetector(0.01, true, false);
#endif
	pAabbAabbCd = new AABBtreeAABBtreeCD(object, obj2);

	setting.m_collisionRadius = 0;
	setting.m_checkBothSidesOfTriangles = true;
	setting.m_checkForNearestCollisionOnly = false;
}

void DemoAabbAabbCd::onSpecialKey( int key, int x, int y )
{
	switch(key)
	{
	case GLUT_KEY_LEFT:
		object->translate(camera->getRightVector() * -MOVE_SPEED);
		break;
	case GLUT_KEY_RIGHT:
		object->translate(camera->getRightVector() * MOVE_SPEED);
		break;
	case GLUT_KEY_UP:
		object->translate(camera->getUpVector() * MOVE_SPEED);
		break;
	case GLUT_KEY_DOWN:
		object->translate(camera->getUpVector() * -MOVE_SPEED);
		break;
	case GLUT_KEY_PAGE_DOWN:
		object->translate(camera->getLookVector() * MOVE_SPEED);
		break;
	case GLUT_KEY_PAGE_UP:
		object->translate(camera->getLookVector() * -MOVE_SPEED);
		break;
	default:
		break;
	}

	if (pAabbAabbCd->computeCollision(recoder, setting))
	{
		printf("***********************************************\n");
		printf("collision detected!\n");
		printf("CDs: %d\n", recoder.m_collisions.size());
		PRINT_VECTOR(recoder.m_nearestCollision.m_adjustedSegmentAPoint);
		recoder.m_collisions.clear();
	}

	glutPostRedisplay();
}

void DemoAabbAabbCd::onKey( unsigned char key, int x, int y )
{
	cVector3d delta(0.0, 0.0, 0.0);

	switch(key) 
	{
	case 27: // ESC
		// case 'x':
		exit(0);
		break;
	case '2':
		{
			bool useWireMode = object->getWireMode();
			object->setWireMode(!useWireMode, true);
			obj2->setWireMode(!useWireMode, true);
		}
		break;
	case '3':
		{
			bool showCollisionTree = object->getShowCollisionTree();

			object->setCollisionDetectorProperties(collisionTreeDisplayLevel, treeColor, true);
			object->setShowCollisionTree(!showCollisionTree, true);

			obj2->setCollisionDetectorProperties(collisionTreeDisplayLevel, treeColor, true);
			obj2->setShowCollisionTree(!showCollisionTree, true);
		}
		break;
	case '-':
		{
			collisionTreeDisplayLevel--;
			if (collisionTreeDisplayLevel < 0) { collisionTreeDisplayLevel = 0; }

			object->setCollisionDetectorProperties(collisionTreeDisplayLevel, treeColor, true);
			object->setShowCollisionTree(true, true);

			obj2->setCollisionDetectorProperties(collisionTreeDisplayLevel, treeColor, true);
			obj2->setShowCollisionTree(true, true);
		}
		break;
	case '+':
	case '=':
		{
			collisionTreeDisplayLevel++;
			object->setCollisionDetectorProperties(collisionTreeDisplayLevel, treeColor, true);
			object->setShowCollisionTree(true, true);

			obj2->setCollisionDetectorProperties(collisionTreeDisplayLevel, treeColor, true);
			obj2->setShowCollisionTree(true, true);
		}
		break;

	case 'a':
		delta = camera->getRightVector() * -MOVE_SPEED;
		break;
	case 'd':
		delta = camera->getRightVector() * MOVE_SPEED;
		break;
	case 's':
		delta = camera->getLookVector() * MOVE_SPEED;
		break;
	case 'w':
		delta = camera->getLookVector() * -MOVE_SPEED;
		break;
	case 'q':
		delta = camera->getUpVector() * MOVE_SPEED;
		break;
	case 'e':
		delta = camera->getUpVector() * -MOVE_SPEED;
		break;
	default:
		break;
	}
	camera->translate(delta);

	glutPostRedisplay();
}

void DemoAabbAabbCd::onMousePress( int button, int state, int x, int y )
{
	lastX = x;
	lastY = y;
	// printf("mouse pressed: button:%d state: %d pos: %d,%d\n", button, state, x, y);
}

void DemoAabbAabbCd::onMousePressMove( int x,int y )
{
	double xRot = (x - lastX) * CAMERA_ROTATE_SPEED;
	double yRot = (y - lastY) * CAMERA_ROTATE_SPEED;

	camera->rotate(camera->getUpVector(), cDegToRad(xRot));
	camera->rotate(camera->getRightVector(), cDegToRad(yRot));
	//printf("mouse press move: %d, %d\n", x, y);
}

void DemoAabbAabbCd::onDisplay()
{
	// render world
	camera->renderView(getWindowWidth(), getWindowHeight());

	// Swap buffers
	glutSwapBuffers();

	// check for any OpenGL errors
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) printf("Error:  %s\n", gluErrorString(err));
}

void DemoAabbAabbCd::onResize( int w, int h )
{
	glViewport(0, 0, w, h);
}
