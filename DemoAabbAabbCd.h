#ifndef DEMO_AABB_AABB_CD_H
#define DEMO_AABB_AABB_CD_H

#include <chai3d.h>

#include "common.h"
#include "GlutApp.h"
#include "AABBtreeAABBtreeCD.h"

class DemoAabbAabbCd : public GlutApp
{
	cWorld* world;

	cCamera* camera;

	cLight* light;

	cMesh* object;

	cMesh* obj2;

	AABBtreeAABBtreeCD* pAabbAabbCd;

	int collisionTreeDisplayLevel;

	cCollisionRecorder recoder;

	cCollisionSettings setting;

	int lastX;
	int lastY;

	virtual void onInit();

	void onSpecialKey(int key, int x, int y);

	void onKey(unsigned char key, int x, int y);

	void onMousePress(int button, int state, int x, int y);

	void onMousePressMove(int x,int y);

	virtual void onDisplay();

	void onResize(int w, int h);
};

#endif