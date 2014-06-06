#ifndef GLUT_APP_H
#define GLUT_APP_H

class GlutApp 
{
public:
	typedef void (*MenuFuncPtr)(void);

	struct MenuEntry 
	{
		int id;
		const char* str;
		MenuFuncPtr fun;
	};

	// 当前 App 实例指针,指向子类实例
	static GlutApp* s_pCurrentApp;

	// 右键菜单 项数最大值
	static const int MAX_MENU = 32;

	// ctor
	GlutApp();

	// getter and setters:
	static void initGlut(int argc, char** argv) { s_argc = argc; s_argv = argv; }

	void setDisplayMode(unsigned int mode) { m_displayMode = mode; }

	void setWindowsSize(int w, int h) { m_winWidth = w; m_winHeight = h; }

	int getWindowWidth() { return m_winWidth; }

	int getWindowHeight() { return m_winHeight; }

	void setWindowsPos(int x, int y) { m_winPosX = x; m_winPosY = y; }

	void setTitle(char *title) { m_title = title; }

	void run();

	void addRightMenu(const char *str, MenuFuncPtr fun);

	// 初始化
	virtual void onInit(){}

	//////////////////////////////////////////////////////////////////////////
	// GLUT delegate callbacks:

	// 空闲函数
	virtual void onIdle(){}

	// 图形显示（OpenGL绘图指令）
	virtual void onDisplay() = 0; // 子类必须重写；不能实例化该类

	// 窗口大小改变
	virtual void onResize(int w, int h){}

	//////////////////////////////////////////////////////////////////////////
	// 键盘事件响应 方法:

	// 一般按键（可打印字符，ESC）
	virtual void onKey(unsigned char key, int x, int y){}

	// 一般按键 按下
	virtual void onKeyDown(unsigned char key, int x, int y) {}

	// 特殊按键（除一般按键外按键）
	virtual void onSpecialKey(int key, int x, int y){}

	// 特殊按键按下
	virtual void onSpecialKeyDown(int key, int x, int y){}

	//////////////////////////////////////////////////////////////////////////
	// 鼠标事件响应 方法:

	// 鼠标按键
	//! @param button: The button parameter is one of GLUT LEFT BUTTON, GLUT MIDDLE BUTTON, or GLUT RIGHT BUTTON.
	//! @param state: The state parameter is either GLUT UP or GLUT DOWN indicating 
	//                 whether the callback was due to a release or press respectively.
	virtual void onMousePress(int button, int state, int x, int y){}

	// 鼠标移动
	virtual void onMouseMove(int x, int y){}

	// 鼠标拖动
	virtual void onMousePressMove(int x,int y){}

	//////////////////////////////////////////////////////////////////////////
	// 定时器相关 方法：
	virtual void onTimer() {}

	void setTimer(int delay, int period = 0);

protected:
	void registerMenus();

	// actual GLUT callback functions:
	static void KeyboardCallback(unsigned char key, int x, int y);

	static void KeyboardUpCallback(unsigned char key, int x, int y);

	static void SpecialKeyboardCallback(int key, int x, int y);

	static void SpecialKeyboardUpCallback(int key, int x, int y);

	static void ReshapeCallback(int w, int h);

	static void IdleCallback();

	static void MouseFuncCallback(int button, int state, int x, int y);

	static void	MotionFuncCallback(int x,int y);

	static void MousePassiveCallback(int x, int y);

	static void DisplayCallback();

	static void MenuCallback(int menuId);
	
	static void TimerCallback(int period);
private:
	unsigned int m_displayMode;

	// for glutInit
	static int s_argc;
	static char** s_argv;

	char *m_title;

	// for glutSetWindowSize
	int m_winWidth;
	int m_winHeight;

	// for windows position
	int m_winPosX;
	int m_winPosY;

	// for menus:
	int       m_menuCount;
	MenuEntry m_menuEntry[MAX_MENU];

	// for timer:
	int m_delay;
	int m_period;
};

#endif // GLUT_APP_H
