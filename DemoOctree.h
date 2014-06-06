#ifndef DEMO_OCTREE_H
#define DEMO_OCTREE_H

#include <chai3d.h>

#include "GlutApp.h"
#include "OctTree.h"

class DemoOctree : public GlutApp
{
	cWorld* world;
	cCamera* camera;
	cLight* light;

	cMesh* mesh1;
	cMesh* mesh2;

	Octree* tree1;
	Octree* tree2;

	cLabel* label;

	cCollisionRecorder cdRecoder;

	int octreeDisplayLevel;

	virtual void onInit();

	virtual void onDisplay();

	virtual void onResize( int w, int h );

	void onKey(unsigned char key, int x, int y);

	virtual void onSpecialKey( int key, int x, int y );
};

#endif // DEMO_OCTREE_H
