//---------------------------------------------------------------------------

#define _USE_MATH_DEFINES

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <set>
#include <memory>

#include <chai3d.h>

#include "common.h"
#include "debug.h"
#include "OctTree.h"

#ifdef COMPARE

//---------------------------------------------------------------------------
// DECLARED CONSTANTS
//---------------------------------------------------------------------------

// initial size (width/height) in pixels of the display window
const int WINDOW_SIZE_W         = 800;
const int WINDOW_SIZE_H         = 600;

// mouse menu options (right button)
const int OPTION_FULLSCREEN     = 1;
const int OPTION_WINDOWDISPLAY  = 2;

const float CAMERA_MOVE_SPEED = 0.05;
const float CAMERA_ROTATE_SPEED = 0.01;

//---------------------------------------------------------------------------
// DECLARED VARIABLES
//---------------------------------------------------------------------------

// a world that contains all objects of the virtual environment
cWorld* world;

// a camera that renders the world in a window display
cCamera* camera;

// a light source to illuminate the objects in the virtual scene
cLight *light;

int count = 0;
int clock_start, clock_end;

// width and height of the current window display
int displayW  = 0;
int displayH  = 0;

// radius of the tool proxy
double proxyRadius;

// a virtual object
cMesh* object;

cMesh* duck;

cShapeSphere* sphere;

cCollisionRecorder cdRecoder;

cCollisionSettings cdSetting;

// display level for collision tree
int collisionTreeDisplayLevel = 1;

// status of the main simulation haptics loop
bool simulationRunning = false;

// simulation clock
cPrecisionClock simClock;

// oct tree handler 八叉树
std::tr1::shared_ptr<Octree> tree;
// Octree* tree;

std::tr1::shared_ptr<Octree> anotherTree;

void resizeWindow(int w, int h);

void keySelect(unsigned char key, int x, int y);

void menuSelect(int value);

void updateGraphics(void);

void testMesh(cMesh* mesh);

void specialKeyHandler(int key, int x, int y);

void timerHandler(int);

int main(int argc, char* argv[])
{
	// create a new world.
	world = new cWorld();
	world->setBackgroundColor(0.0, 0.0, 0.0);

	// create a camera and insert it into the virtual world
	camera = new cCamera(world);
	world->addChild(camera);

	// position and oriente the camera
	camera->set( cVector3d (3.0, -10.0, 2.0),    // camera position (eye)
		cVector3d (3.0, 0.0, 0.0),    // lookat position (target)
		cVector3d (0.0, 1.0, 0.0));   // direction of the "up" vector

	// set the near and far clipping planes of the camera
	// anything in front/behind these clipping planes will not be rendered
	camera->setClippingPlanes(0.01, 20.0);

	// create a light source and attach it to the camera
	light = new cLight(world);
	camera->addChild(light);                   // attach light to camera
	light->setEnabled(true);                   // enable light source
	light->setPos(cVector3d( 2.0, 0.5, 1.0));  // position the light source
	light->setDir(cVector3d(-2.0, 0.5, 1.0));  // define the direction of the light beam

	proxyRadius = 0.01;

	// create a virtual mesh
	object = new cMesh(world);

	// add object to world
	world->addChild(object);

	// set the position of the object at the center of the world
	object->setPos(0.0, 0.0, 0.0);

	clock_start = clock();

	// load an object file
	bool fileload = object->loadFromFile("duck.obj");
	// object->scale(0.1);
	if (!fileload)
	{
		printf("Error - 3D Model failed to load correctly.\n");
		return (-1);
	}

	object->setUseMaterial(true);

	testMesh(object);

	//// another object
	//cMesh* another = new cMesh(*object);
	cMesh* another = new cMesh(world);

	another->loadFromFile("duck.obj");
	
	another->setParentWorld(world);
	
	world->addChild(another);
	
	another->setPos(6.0, 0.0, 0.0);

//	another->rotate(cVector3d(0, 0, 1), M_PI);

	// duck
	duck = new cMesh(world);
	
	duck->setPos(3, 0, 0);
	
	duck->loadFromFile("duck.obj");
	
	duck->setUseMaterial(true);
	
	world->addChild(duck);
	
	PRINT_VECTOR(duck->getPos());

	// shpere
	sphere = new cShapeSphere(0.1);

	sphere->setPos(1.5, 0, 0);
	
	world->addChild(sphere);

	// collsion settings
	cdSetting.m_returnMinimalCollisionData = true;
	cdSetting.m_collisionRadius = sphere->getRadius();

	// OctTree HERE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// tree = new OctTree(duck, 0.001); // 暴力法构造太慢
	// tree = new Octree(duck, 0.01);
	tree.reset(new Octree(duck, 0.05));
	tree->build();
	//tree->validate();
	//tree->setDisplayDepth(1);
	// tree->setDisplayDepth(5);
	// world->addChild(tree);

	anotherTree.reset(new Octree(another, 0.05));
	anotherTree->build();

#if 0
	// monkey
	cMesh* monkey = new cMesh(world);
	monkey->setPos(4, 0, 0);
	monkey->loadFromFile("suzanne.obj");
	world->addChild(monkey);

	// sphere
	cShapeSphere* sphere = new cShapeSphere(.5);
	sphere->setPos(-2.0, 0.0, 0.0);
	sphere->setParent(world);
	world->addChild(sphere);

	// lines
	cVector3d origin(0.0, 0.0, 0.0);
	cShapeLine* line1 = new cShapeLine(origin, cVector3d(1, 0, 0));
	world->addChild(line1);

	cLabel* xLabel = new cLabel();
	xLabel->m_string = "X";
	xLabel->setPos(1, 0, 0);
	world->addChild(xLabel);

	cShapeLine* line2 = new cShapeLine(origin, cVector3d(0, 1, 0));
	world->addChild(line2);
	cLabel* yLabel = new cLabel();
	yLabel->m_string = "Y";
	yLabel->setPos(0, 1, 0);
	world->addChild(yLabel);

	cShapeLine* line3 = new cShapeLine(origin, cVector3d(0, 0, 1));
	world->addChild(line3);
	cLabel* zLabel = new cLabel();
	zLabel->m_string = "Z";
	zLabel->setPos(0, 0, 1);
	world->addChild(zLabel);
#endif

	clock_end = clock();

	printf("time elapse: %d ms\n", clock_end - clock_start);

	// compute a boundary box
	object->computeBoundaryBox(true);

	// compute collision detection algorithm
	object->createAABBCollisionDetector(1.01 * proxyRadius, true, false);
	// object->createSphereTreeCollisionDetector(0.01, true, false);
	
	PRINT_VECTOR(camera->getRightVector());
	PRINT_VECTOR(camera->getLookVector());
	PRINT_VECTOR(camera->getUpVector());

	// initialize GLUT
	glutInit(&argc, argv);

	// retrieve the resolution of the computer display and estimate the position
	// of the GLUT window so that it is located at the center of the screen
	int screenW = glutGet(GLUT_SCREEN_WIDTH);
	int screenH = glutGet(GLUT_SCREEN_HEIGHT);
	int windowPosX = (screenW - WINDOW_SIZE_W); // / 2;
	int windowPosY = (screenH - WINDOW_SIZE_H) / 2;

	// initialize the OpenGL GLUT window
	glutInitWindowPosition(windowPosX, windowPosY);
	glutInitWindowSize(WINDOW_SIZE_W, WINDOW_SIZE_H);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	glutCreateWindow(argv[0]);
	glutDisplayFunc(updateGraphics);
	glutReshapeFunc(resizeWindow);
	glutSetWindowTitle("CHAI 3D");

	glutKeyboardFunc(keySelect);
	glutSpecialFunc(specialKeyHandler);

	glutTimerFunc(100, timerHandler, 0);

	// create a mouse menu (right button)
	glutCreateMenu(menuSelect);
	glutAddMenuEntry("full screen", OPTION_FULLSCREEN);
	glutAddMenuEntry("window display", OPTION_WINDOWDISPLAY);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	//-----------------------------------------------------------------------
	// START SIMULATION
	//-----------------------------------------------------------------------

	// simulation in now running
	simulationRunning = true;

	// start the main graphics rendering loop
	glutMainLoop();

	int clock_ending = clock();
	printf("FPS: %g\n", count / (clock_ending - clock_end));
	// exit
	return (0);
}

//---------------------------------------------------------------------------

void resizeWindow(int w, int h)
{
	// update the size of the viewport
	displayW = w;
	displayH = h;
	glViewport(0, 0, displayW, displayH);
}

//////////////////////////////////////////////////////////////////////////
void keySelect(unsigned char key, int x, int y)
{
	cVector3d delta(1.0, 1.0, 1.0);
	bool collid = false;

	// printf("key %c(%d) %d, %d\n", key, key, x, y);
	switch(key) {
	case 27:
	case 'x':
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

			tree->setDisplayDepth(collisionTreeDisplayLevel);
			anotherTree->setDisplayDepth(collisionTreeDisplayLevel);
		}
		break;
	case '+':
		{
			collisionTreeDisplayLevel++;
			cColorf tmpColor = cColorf(1.0, 0.0, 0.0);
			object->setCollisionDetectorProperties(collisionTreeDisplayLevel, tmpColor, true);
			object->setShowCollisionTree(true, true);

			tree->setDisplayDepth(collisionTreeDisplayLevel);
			anotherTree->setDisplayDepth(collisionTreeDisplayLevel);
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

	// I J K L to move duck
	case 'i':
		delta = camera->getLookVector() * -CAMERA_MOVE_SPEED;
		duck->translate(delta);
		tree->translate(delta);
		collid = tree->computeCollision(anotherTree.get(), cdRecoder);
		break;
	case 'k':
		delta = camera->getLookVector() * CAMERA_MOVE_SPEED;
		duck->translate(delta);
		tree->translate(delta);
		collid = tree->computeCollision(anotherTree.get(), cdRecoder);
		break;
	case 'j':
		delta = camera->getRightVector() * -CAMERA_MOVE_SPEED;
		duck->translate(delta);
		tree->translate(delta);
		collid = tree->computeCollision(anotherTree.get(), cdRecoder);
		break;
	case 'l':
		delta = camera->getRightVector() * CAMERA_MOVE_SPEED;
		duck->translate(delta);
		tree->translate(delta);
		collid = tree->computeCollision(anotherTree.get(), cdRecoder);
		break;
	default:
		break;
	}

	if (collid)
	{
		printf("collision detected! %d\n", clock());
	}

	// PRINT_VECTOR(camera->getGlobalPos());
}

//////////////////////////////////////////////////////////////////////////
// 特殊按键
void specialKeyHandler(int key, int x, int y)
{
//	printf("special key: %d (%c), %d, %d\n", key, key, x, y);
	static cVector3d lastPos;
	cVector3d delta;
	switch(key) {
#if 0
	// 上下左右控制相机旋转
	case GLUT_KEY_UP:
		camera->rotate(camera->getRightVector(), CAMERA_ROTATE_SPEED);
		break;
	case GLUT_KEY_DOWN:
		camera->rotate(camera->getRightVector(), -CAMERA_ROTATE_SPEED);
		break;
	case GLUT_KEY_LEFT:
		camera->rotate(camera->getUpVector(), CAMERA_ROTATE_SPEED);
		break;
	case GLUT_KEY_RIGHT:
		camera->rotate(camera->getUpVector(), -CAMERA_ROTATE_SPEED);
		break;
#endif
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
	if (sphere)
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

		lastPos = sphere->getPos();
	}
}

//////////////////////////////////////////////////////////////////////////
void menuSelect(int value)
{
	switch (value)
	{
		// enable full screen display
	case OPTION_FULLSCREEN:
		glutFullScreen();
		break;

		// reshape window to original size
	case OPTION_WINDOWDISPLAY:
		glutReshapeWindow(WINDOW_SIZE_W, WINDOW_SIZE_H);
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
// 鼠标移动
void passiveMotionHandler(int x, int y)
{
	// printf("passive motion to %d, %d\n", x, y);
	
}

//////////////////////////////////////////////////////////////////////////
void updateGraphics(void)
{
	count++;
	//tree->render();

	// render world
	camera->renderView(displayW, displayH);

	tree->render();

	anotherTree->render();

	// Swap buffers
	glutSwapBuffers();

	// check for any OpenGL errors
	GLenum err;
	err = glGetError();
	if (err != GL_NO_ERROR) printf("Error:  %s\n", gluErrorString(err));

	// inform the GLUT window to call updateGraphics again (next frame)
	if (simulationRunning)
	{
		glutPostRedisplay();
	}
}

//////////////////////////////////////////////////////////////////////////
void testMesh( cMesh* mesh )
{
	if(mesh)
	{
		cMesh* allMeshes = dynamic_cast<cMesh*>(mesh->getChild(0));

		assert(allMeshes);

		int numChildren = allMeshes->getNumChildren();
		int numVertices = allMeshes->getNumVertices(true);
		int numTriangles = allMeshes->getNumTriangles(true);

		printf("Mesh %p: \nVertices: %d\nTriangles: %d\n", mesh, numVertices, numTriangles);
		
		for(int i=0; i<numChildren; ++i)
		{
			printf("child[%d]: ", i);

			cMesh* child =  dynamic_cast<cMesh*>(allMeshes->getChild(i));
			assert(child);
			printf("%d\t%d\n", child->getNumVertices(true), child->getNumTriangles(true));
		}
	}
}

void timerHandler( int rotate )
{
	
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
struct vec3less
{
	bool operator()(const cVector3d& a, const cVector3d& b) const
	{
		if(a.x - b.x <= DBL_EPSILON) return true;
		if(fabs(a.x - b.x) < DBL_EPSILON && a.y - b.y <= DBL_EPSILON)
			return true;
		if(fabs(a.x - b.x) < DBL_EPSILON && fabs(a.y - b.y) < DBL_EPSILON && a.z - b.y <= DBL_EPSILON)
			return true;
		return false;
	}
};

void testVec3()
{
	vector<cVector3d> vVec;
	std::set<cVector3d, vec3less> vSet;

	vec3less comp;

	srand(time(NULL));
	int start = clock();	
	while(clock() - start < 1000 && vVec.size() < vVec.max_size()) {
		double r1 = rand() / (double)RAND_MAX;
		double r2 = rand() / (double)RAND_MAX;
		double r3 = rand() / (double)RAND_MAX;

		cVector3d randVec(r1, r2, r3);

		double r4 = rand() / (double)RAND_MAX;
		double r5 = rand() / (double)RAND_MAX;
		double r6 = rand() / (double)RAND_MAX;
		cVector3d randVec2(r4, r5, r6);

		assert(!(comp(randVec, randVec2) && comp(randVec2, randVec)));
		printf("pass\n");

		vVec.push_back(randVec);
		//vSet.insert(randVec);
	}
	std::cout << "generated: " << vVec.size() << "\n";
	std::cout << "inserted: " << vSet.size() << "\n";

	std::cout << "sort...";
	sort(vVec.begin(), vVec.end(), vec3less());
	std::cout << "done.\n";

	std::cout << "checking...";

	std::vector<cVector3d>::iterator vit = vVec.begin();
	std::set<cVector3d, vec3less>::iterator sit = vSet.begin();
	for( ; vit != vVec.end(); ++vit, ++sit)
	{
		if (vit->equals(*sit, DBL_EPSILON))
		{
			std::cout << "not equals\n";
		}
	}
}

#endif // COMPARE