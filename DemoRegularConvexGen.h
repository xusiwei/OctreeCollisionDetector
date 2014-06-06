#ifndef DEMO_REGULAR_CONVEX_GEN_H
#define DEMO_REGULAR_CONVEX_GEN_H

#include "common.h"
#include "GlutApp.h"
#include "RegularConvexGenerator.h"

class DemoRegularConvexGen : public GlutApp
{
	Convex* pConvex;

	RegularConvexGenerator* generator;

	float angle;

	int depth;

	virtual void onKey( unsigned char key, int x, int y );

	virtual void onTimer();

	virtual void onInit();

	void onResize(int w, int h);

	virtual void onDisplay();

};

#endif