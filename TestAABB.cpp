#include <windows.h>
#include <gl/GL.h>
#include <gl/glut.h>

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <stdio.h>

#include <memory>

#include <chai3d.h>

#include "common.h"

#include "GlutApp.h"

#ifdef TEST_AABB

int displayW;

int displayH;

int collisionTreeDisplayLevel;

class TestAABB : public GlutApp
{
	cWorld* world;

	cCamera* camera;

	cLight* light;

	cMesh* object;

	virtual void onInit() 
	{
		printf("OnInit\n");
		// 创建 world.
		world = new cWorld();
		world->setBackgroundColor(0, 0, 0);

		// 创建 camera
		camera = new cCamera(world);
		world->addChild(camera);

		// 摆放相机
		camera->set( cVector3d (3.0, -10.0, 2.0),    // camera position (eye)
			cVector3d (3.0, 0.0, 0.0),    // lookat position (target)
			cVector3d (0.0, 1.0, 0.0));   // direction of the "up" vector

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
		
		if ( ! object->loadFromFile("duck.obj") )
		{
			printf("Error - 3D Model failed to load correctly.\n");
			exit(-1);
		}
		object->setUseMaterial(true);
		object->computeBoundaryBox(true);

		// 添加 AABB 碰撞检测器
		object->createAABBCollisionDetector( 0.01, true, false);
		
	}

	void onKey(unsigned char key, int x, int y)
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
		default:
			break;
		}
	}

	virtual void onTimer() 
	{
		//static int count = 0;
		//printf("Alarm %d!\n", count++);

		object->rotate(camera->getUpVector(), glm::radians(5.0));
		glutPostRedisplay();
	}

	virtual void onDisplay() 
	{
		// render world
		camera->renderView(displayW, displayH);

		// Swap buffers
		glutSwapBuffers();

		// check for any OpenGL errors
		GLenum err = glGetError();
		if (err != GL_NO_ERROR) printf("Error:  %s\n", gluErrorString(err));
	}

	void onResize(int w, int h)
	{
		displayW = w;
		displayH = h;
		glViewport(0, 0, displayW, displayH);
	}
};

int main(int argc, char** argv)
{
	GlutApp* app = new TestAABB();

	displayW = 800;
	displayH = 600;

	app->initGlut(argc, argv);
	app->setTitle("AABB test");
	app->setWindowsSize(displayW, displayH);

	app->setTimer(1000, 100);

	app->run();

	return 0;
}

#endif // TEST_AABB



