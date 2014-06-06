#ifndef DEMO_CHAI3D_CD_H
#define DEMO_CHAI3D_CD_H

#include "common.h"
#include "chai3d.h"
#include "GlutApp.h"

class DemoChai3dCd : public GlutApp
{

	cWorld* world;

	cCamera* camera;

	cLight *light;

	// a virtual object
	cMesh* object;

	cShapeSphere* sphere;

	cCollisionRecorder cdRecoder;

	cCollisionSettings cdSetting;

	// display level for collision tree
	int collisionTreeDisplayLevel;

	void onInit();

	virtual void onSpecialKey( int key, int x, int y );

	virtual void onDisplay();

	virtual void onResize( int w, int h );

	virtual void onKey( unsigned char key, int x, int y );
};

#endif