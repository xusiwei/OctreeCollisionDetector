#include <gl/glut.h>
#include <assert.h>
#include <stdio.h>

#include "GlutApp.h"

int GlutApp::s_argc = 0;

char** GlutApp::s_argv = 0;

GlutApp* GlutApp::s_pCurrentApp = 0;

int g_iLastWindow = 0;

void GlutApp::run()
{
	GlutApp* lastApp = GlutApp::s_pCurrentApp;
	GlutApp::s_pCurrentApp = this;

	GlutApp* app = GlutApp::s_pCurrentApp;
	assert(app);

	int screenW = glutGet(GLUT_SCREEN_WIDTH);
	int screenH = glutGet(GLUT_SCREEN_HEIGHT);

	if (!app->m_winWidth)
	{
		app->m_winWidth = screenW / 2;
		app->m_winHeight = screenH / 2;
	}
	
	if (!app->m_winPosX)
	{
		app->m_winPosX = (screenW - app->m_winWidth) / 2;
		app->m_winPosY = (screenH - app->m_winHeight) / 2;
	}

	if (!lastApp) // first time calling Glut::run().
	{
		// glutInit that should only be called exactly once in a GLUT program.
		glutInit(&this->s_argc, this->s_argv);
#if 0
		glutCreateWindow(app->m_title);
		g_iLastWindow = glutGetWindow();
		printf("create window: %d\n", g_iLastWindow); // debug [6/2/2014 xu]

		// glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
		glutInitDisplayMode(this->mDisplayMode);
		glutPositionWindow(app->m_winPosX, app->m_winPosY);
		glutReshapeWindow(app->m_winWidth, app->m_winHeight); // 会调用一次 reshape callback
#else
		glutInitDisplayMode(this->m_displayMode);
		glutInitWindowPosition(app->m_winPosX, app->m_winPosY);
		glutInitWindowSize(app->m_winWidth, app->m_winHeight);

		glutCreateWindow(app->m_title);
		g_iLastWindow = glutGetWindow();
		printf("create window: %d\n", g_iLastWindow); // debug [6/2/2014 xu]
#endif
	}
	else
	{
		glutDestroyWindow(g_iLastWindow);
#if 0
		glutCreateWindow(app->m_title);
		g_iLastWindow = glutGetWindow();
		printf("create window: %d\n", g_iLastWindow); // debug [6/2/2014 xu]

		glutInitDisplayMode(this->mDisplayMode);
		glutPositionWindow(app->m_winPosX, app->m_winPosY);
		glutReshapeWindow(app->m_winWidth, app->m_winHeight);
#else
		glutInitDisplayMode(this->m_displayMode);
		glutInitWindowPosition(app->m_winPosX, app->m_winPosY);
		glutInitWindowSize(app->m_winWidth, app->m_winHeight);

		glutCreateWindow(app->m_title);
		g_iLastWindow = glutGetWindow();
		printf("create window: %d\n", g_iLastWindow); // debug [6/2/2014 xu]
#endif
	}

	app->onInit();

	// register keyboard callbacks
	glutKeyboardFunc(GlutApp::KeyboardCallback);
	glutKeyboardUpFunc(GlutApp::KeyboardUpCallback);
	glutSpecialFunc(GlutApp::SpecialKeyboardCallback);
	glutSpecialUpFunc(GlutApp::SpecialKeyboardUpCallback);

	// register mouse callbacks
	glutMouseFunc(GlutApp::MouseFuncCallback);
	glutMotionFunc(GlutApp::MotionFuncCallback);
	glutPassiveMotionFunc(GlutApp::MousePassiveCallback);

	// register menus:
	registerMenus();

	// regitser windows resize callback
	glutReshapeFunc(GlutApp::ReshapeCallback);

	// register render callback
	glutDisplayFunc(GlutApp::DisplayCallback);

	// register timer callbacks:
	if (app->m_delay)
	{
		glutTimerFunc(app->m_delay, GlutApp::TimerCallback, app->m_period);
	}

	// register idle callback
	glutIdleFunc(GlutApp::IdleCallback);

	GlutApp::IdleCallback();

	glutMainLoop();
}

GlutApp::GlutApp()
{
	m_displayMode = GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL;
	m_menuCount = 0;
	m_delay = 0;
	m_period = 0;

	m_winPosX = 0;
	m_winPosY = 0;
	m_winWidth = 0;
	m_winHeight = 0;
}

void GlutApp::KeyboardCallback( unsigned char key, int x, int y )
{
	GlutApp::s_pCurrentApp->onKey(key,x,y);
}

void GlutApp::KeyboardUpCallback( unsigned char key, int x, int y )
{
	GlutApp::s_pCurrentApp->onKeyDown(key,x,y);
}

void GlutApp::SpecialKeyboardCallback( int key, int x, int y )
{
	GlutApp::s_pCurrentApp->onSpecialKey(key,x,y);
}

void GlutApp::SpecialKeyboardUpCallback( int key, int x, int y )
{
	GlutApp::s_pCurrentApp->onSpecialKeyDown(key,x,y);
}

void GlutApp::ReshapeCallback( int w, int h )
{
	GlutApp::s_pCurrentApp->setWindowsSize(w, h);
	GlutApp::s_pCurrentApp->onResize(w,h);
}

void GlutApp::IdleCallback()
{
	GlutApp::s_pCurrentApp->onIdle();
}

void GlutApp::MouseFuncCallback( int button, int state, int x, int y )
{
	GlutApp::s_pCurrentApp->onMousePress(button,state,x,y);
}

void GlutApp::MotionFuncCallback( int x,int y )
{
	GlutApp::s_pCurrentApp->onMousePressMove(x,y);
}

void GlutApp::MousePassiveCallback( int x, int y )
{
	GlutApp::s_pCurrentApp->onMouseMove(x, y);
}

void GlutApp::DisplayCallback( void )
{
	GlutApp::s_pCurrentApp->onDisplay();
}

void GlutApp::addRightMenu( const char *str, MenuFuncPtr fun )
{
	m_menuEntry[m_menuCount].id = m_menuCount;
	m_menuEntry[m_menuCount].str = str;
	m_menuEntry[m_menuCount].fun = fun;
	m_menuCount++;
}

void GlutApp::registerMenus()
{
	if (m_menuCount > 0)
	{
		glutCreateMenu(GlutApp::MenuCallback);
		for (int i=0; i<m_menuCount; ++i)
		{
			glutAddMenuEntry(m_menuEntry[i].str, m_menuEntry[i].id);
		}
		glutAttachMenu(GLUT_RIGHT_BUTTON);
	}
}

void GlutApp::MenuCallback( int menuId )
{
	for (int i=0; i<GlutApp::s_pCurrentApp->m_menuCount; ++i)
	{
		if (menuId == GlutApp::s_pCurrentApp->m_menuEntry[i].id)
		{
			GlutApp::s_pCurrentApp->m_menuEntry[i].fun();
		}
	}
}

void GlutApp::setTimer( int delay, int period )
{
	this->m_delay = delay;
	this->m_period = period;
}

void GlutApp::TimerCallback( int period )
{
	// printf("Timer Alarm!\n");
	GlutApp::s_pCurrentApp->onTimer();
	if (period)
	{
		glutTimerFunc(period, GlutApp::TimerCallback, period);
	}
}
