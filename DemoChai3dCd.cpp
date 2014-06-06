#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <set>
#include <memory>

#include "DemoChai3dCd.h"

static const float CAMERA_MOVE_SPEED = 0.05;
static const float CAMERA_ROTATE_SPEED = 0.01;

#ifdef TEST_DEMO_CHAI3DCD

int main(int argc, char* argv[])
{
	GlutApp *pTest = new DemoChai3dCd();

	pTest->initGlut(argc, argv);
	pTest->setTitle("Chai3d collsion detection test");
	pTest->setWindowsSize(640, 480);

	pTest->run();

	return 0;
}

#endif //  CHAI3DCD_TEST

void DemoChai3dCd::onInit()
{
	collisionTreeDisplayLevel = 0;

	// 创建 world.
	world = new cWorld();
	world->setBackgroundColor(0.0, 0.0, 0.0);

	// 创建 camera
	camera = new cCamera(world);
	world->addChild(camera);

	// 摆放相机
	camera->set( cVector3d (0.0, -5.0, 2.0),    // camera position (eye)
		cVector3d (0.0, 0.0, 0.0),    // lookat position (target)
		cVector3d (0.0, 1.0, 0.0));   // direction of the "up" vector

	// 设置相机剪切平面
	// anything in front/behind these clipping planes will not be rendered
	camera->setClippingPlanes(0.01, 20.0);

	// 创建光源 同时，将光源添加到相机
	light = new cLight(world);
	camera->addChild(light);                   // attach light to camera
	light->setEnabled(true);                   // enable light source
	light->setPos(cVector3d( 2.0, 0.5, 1.0));  // position the light source
	light->setDir(cVector3d(-2.0, 0.5, 1.0));  // define the direction of the light beam

	// 创建 mesh
	object = new cMesh(world);

	// 加入 world
	world->addChild(object);

	// 设定位置
	object->setPos(0.0, 0.0, 0.0);

	// load an object file
	bool fileload = object->loadFromFile("duck.obj");
	if (!fileload)
	{
		printf("Error - 3D Model failed to load correctly.\n");
		exit(-1);
	}

	object->setUseMaterial(true);

	// shpere
	sphere = new cShapeSphere(0.1);

	sphere->setPos(1.5, 0, 0);

	world->addChild(sphere);

	// collsion settings
	cdSetting.m_returnMinimalCollisionData = true;
	cdSetting.m_collisionRadius = sphere->getRadius();

	// compute a boundary box
	object->computeBoundaryBox(true);

	// 添加 AABB 碰撞检测器
	object->createAABBCollisionDetector( 0.01, true, false);
	// object->createSphereTreeCollisionDetector(0.01, true, false);

	PRINT_VECTOR(camera->getRightVector());
	PRINT_VECTOR(camera->getLookVector());
	PRINT_VECTOR(camera->getUpVector());
}

void DemoChai3dCd::onSpecialKey( int key, int x, int y )
{
	static cVector3d lastPos;
	static int inited = 0;
	cVector3d delta;
	switch(key) {
	case GLUT_KEY_UP:
		delta = camera->getUpVector() * CAMERA_MOVE_SPEED;
		break;
	case GLUT_KEY_DOWN:
		delta = camera->getUpVector() * -CAMERA_MOVE_SPEED;
		break;
	case GLUT_KEY_LEFT:
		delta = camera->getRightVector() * -CAMERA_MOVE_SPEED;
		break;
	case GLUT_KEY_RIGHT:
		delta = camera->getRightVector() * CAMERA_MOVE_SPEED;
		break;
	case GLUT_KEY_PAGE_UP:
		delta = camera->getLookVector() * -CAMERA_MOVE_SPEED;
		break;
	case GLUT_KEY_PAGE_DOWN:
		delta = camera->getLookVector() * CAMERA_MOVE_SPEED;
		break;
	default:
		break;
	}
	if (sphere && inited)
	{
		sphere->translate(delta);
		cVector3d currPos = sphere->getPos();

		bool collide = object->computeCollisionDetection(lastPos, currPos, cdRecoder, cdSetting);
		if (collide)
		{
			printf("collision detected!\n");
			PRINT_VECTOR(sphere->getPos());
			PRINT_VECTOR(cdRecoder.m_nearestCollision.m_localPos);
		}
	}
	lastPos = sphere->getPos();
	inited = 1;
}

void DemoChai3dCd::onDisplay()
{
	// render world
	camera->renderView(getWindowWidth(), getWindowHeight());

	// Swap buffers
	glutSwapBuffers();

	// check for any OpenGL errors
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) printf("Error:  %s\n", gluErrorString(err));

	glutPostRedisplay();
}

void DemoChai3dCd::onResize( int w, int h )
{
	glViewport(0, 0, w, h);
}

void DemoChai3dCd::onKey( unsigned char key, int x, int y )
{
	cVector3d delta(1.0, 1.0, 1.0);

	switch(key) {
	case 27: // ESC
		// case 'x':
		exit(0);
		break;
	case '2':
		{
			bool useWireMode = object->getWireMode();
			object->setWireMode(!useWireMode, true);
		}
		break;
	case '3':
		{
			cColorf tmpColor = cColorf(1.0, 0.0, 0.0);
			object->setCollisionDetectorProperties(collisionTreeDisplayLevel, tmpColor, true);
			bool showCollisionTree = object->getShowCollisionTree();
			object->setShowCollisionTree(!showCollisionTree, true);
		}
		break;
	case '-':
		{
			collisionTreeDisplayLevel--;
			if (collisionTreeDisplayLevel < 0) { collisionTreeDisplayLevel = 0; }
			cColorf tmpColor = cColorf(1.0, 0.0, 0.0);
			object->setCollisionDetectorProperties(collisionTreeDisplayLevel, tmpColor, true);
			object->setShowCollisionTree(true, true);
		}
		break;
	case '+':
		{
			collisionTreeDisplayLevel++;
			cColorf tmpColor = cColorf(1.0, 0.0, 0.0);
			object->setCollisionDetectorProperties(collisionTreeDisplayLevel, tmpColor, true);
			object->setShowCollisionTree(true, true);
		}
		break;

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
