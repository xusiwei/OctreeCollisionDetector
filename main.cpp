#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <stdio.h>

#include "common.h"
#include "GlutApp.h"

#include "DemoOctree.h"
#include "DemoChai3dCd.h"
#include "DemoAabbAabbCd.h"
#include "DemoRegularConvexGen.h"

#ifdef MAIN

GlutApp* appOctree;
GlutApp* appChai3dCd;
GlutApp* appAabbAabbCd;
GlutApp* appRegularConvexGen;

class MainApp : public GlutApp
{
	virtual void onDisplay() 
	{
		
	}

	virtual void onKeyDown( unsigned char key, int x, int y ) 
	{
		switch (key) 
		{
		case 1:
			appChai3dCd->run();
			break;
		case 2:
			appOctree->run();
			break;
		case 3:
			appAabbAabbCd->run();
			break;
		case 4:
			appRegularConvexGen->run();
			break;
		default:
			break;
		}
	}
};

int main(int argc, char **argv)
{
	GlutApp::initGlut(argc, argv);

	appOctree = new DemoOctree();
	appOctree->setWindowsSize(640, 480);
	appOctree->setTitle("Octree collision detection");

	appChai3dCd = new DemoChai3dCd();
	appOctree->setWindowsSize(640, 480);
	appOctree->setTitle("Chai3d collision detection");

	appAabbAabbCd = new DemoAabbAabbCd();
	appOctree->setWindowsSize(800, 600);
	appOctree->setTitle("AABB tree - AABB tree collision detection");

	appRegularConvexGen = new DemoRegularConvexGen();
	appOctree->setWindowsSize(800, 600);
	appOctree->setTitle("Regular convex generanation algorithm");

	appChai3dCd->run();

	return 0;
}

#endif
