/*
 * testGLUTevent.cpp
 *
 *  Created on: 2014年5月11日
 *      Author: xu
 */

#include <GL/glut.h>
#include <stdio.h>

#include "common.h"

#ifdef GLUT_TEST

void display() {}

// 一般按键（所有可打印字符，ESC也在内）
void keyboardHander(unsigned char ch, int x, int y)
{
	printf("key %d(%c) x: %d, y: %d\n", ch, ch, x, y);
	fflush(stdout);
}

// 特殊按键
void specialKeyHandler(int key, int x, int y)
{
	printf("special key");

	switch(key) {
	case GLUT_KEY_UP:
		printf("%d(%s) ", key, "GLUT_KEY_UP");
		break;
	case GLUT_KEY_DOWN:
		printf("%d(%s) ", key, "GLUT_KEY_DOWN");
		break;
	case GLUT_KEY_LEFT:
		printf("%d(%s) ", key, "GLUT_KEY_LEFT");
		break;
	case GLUT_KEY_RIGHT:
		printf("%d(%s) ", key, "GLUT_KEY_RIGHT");
		break;
	default:
		printf("%d(%s) ", key, "Other Special keys");
	}
	printf("x: %d, y: %d\n", x, y);
	fflush(stdout);
}

// 鼠标按键
void mouseHandler(int button, int state, int x, int y)
{
	printf("mouse pos: (%3d, %3d) button: %s(%d), state: %s(%d)\n", x, y,
				GLUT_LEFT_BUTTON == button ? "GLUT_LEFT_BUTTON"
				: GLUT_RIGHT_BUTTON == button ? "GLUT_RIGHT_BUTTON"
				: GLUT_MIDDLE_BUTTON == button ? "GLUT_MIDDLE_BUTTON"
				: "UNKOW"
			, button,
				GLUT_UP == state ? "GLUT_UP"
				: GLUT_DOWN == state ? "GLUT_DOWN"
				: "UNKNOW"
			, state
			);
	fflush(stdout);
}

// 鼠标拖动
void motionHandler(int x, int y)
{
	printf("motion to %d, %d\n", x, y);
	fflush(stdout);
}

// 鼠标移动
void passiveMotionHandler(int x, int y)
{
	printf("passive motion to %d, %d\n", x, y);
	fflush(stdout);
}

void testTimer(int i)
{
	printf("Alarm %d\n", i);
	fflush(stdout);
	if( i < 5 )
		glutTimerFunc(1000, testTimer, i+1);
}


int main(int argc, char *argv[])
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
	glutInitWindowSize(400, 300);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Getting started with OpenGL 4.3");

	glutDisplayFunc(display);

	glutKeyboardFunc(keyboardHander); // 键盘按键(一般)
	glutSpecialFunc(specialKeyHandler); // 特殊按键

	glutMouseFunc(mouseHandler); // 鼠标按键
	glutMotionFunc(motionHandler);  // 鼠标拖动
	glutPassiveMotionFunc(passiveMotionHandler); // 鼠标移动

	glutTimerFunc(1000, testTimer, 1); // 定时器

	glutMainLoop(); // start main loop.
	return 0;
}


#endif // GLUT_TEST


