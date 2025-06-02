#include "iGraphics.h"

//function declarations
void startScreen();

//button states
//start screen
int play_button, settings_button, exit_button;

int currentScreen; //0 means star screen

void iDraw()
{
	iClear();

	switch (currentScreen) {
		case 0:
			startScreen();
			break;
		
		default:
			break;
	}
}

//mouse move
void iMouseMove(int mx, int my) {
	switch (currentScreen) {
		case 0:			
			if ((mx > 265 && mx < 535) && (my > 280 && my < 340)) play_button = 1; else play_button = 0;
			if ((mx > 265 && mx < 535) && (my > 218 && my < 278)) settings_button = 1; else settings_button = 0;
			break;
		
		default:
			break;
	}
}

//mouse drag and move
void iMouseDrag(int mx, int my) {
    // place your codes here
}

/*
function iMouse() is called when the user presses/releases the mouse.
(mx, my) is the position where the mouse pointer is.
*/
void iMouse(int button, int state, int mx, int my)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
			switch (currentScreen) {
				case 0:
					if (play_button) play_button = 2;
					if (settings_button) settings_button = 2;
					break;
				
				default:
					break;
			}
    }
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        // place your codes here
    }
}

/*
dir = 1 for up, -1 for down.
*/
void iMouseWheel(int dir, int mx, int my)
{
    // place your code here
}

void iKeyboard(unsigned char key)
{
	switch (key) {
    case 'q':
			// do something with 'q'
			break;
			
    default:
			break;
    }
}

/*
GLUT_KEY_F1, GLUT_KEY_F2, GLUT_KEY_F3, GLUT_KEY_F4, GLUT_KEY_F5, GLUT_KEY_F6,
GLUT_KEY_F7, GLUT_KEY_F8, GLUT_KEY_F9, GLUT_KEY_F10, GLUT_KEY_F11,
GLUT_KEY_F12, GLUT_KEY_LEFT, GLUT_KEY_UP, GLUT_KEY_RIGHT, GLUT_KEY_DOWN,
GLUT_KEY_PAGE_UP, GLUT_KEY_PAGE_DOWN, GLUT_KEY_HOME, GLUT_KEY_END,
GLUT_KEY_INSERT
*/
void iSpecialKeyboard(unsigned char key)
{
	switch (key) {
		case GLUT_KEY_END:
			// do something
			break;

		default:
			break;
	}
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	currentScreen = 0;
	iInitialize(800, 500, "Echo Caves");
	return 0;
}



/**
 * Define screens here..........
 * 
 * Declare them on top first...
 */


void startScreen() {
	iShowImage(177, 385, "assets/title_logo.png");

	switch (play_button) {
		case 0:
			iShowImage(265, 280, "assets/buttons/play_button.png");
			break;
		
		case 1:
			iShowImage(265, 280, "assets/buttons/play_button_hover.png");
			break;

		case 2:
			iShowImage(265, 280, "assets/buttons/play_button_pressed.png");
			break;
		default:
			break;
	}

	switch (settings_button) {
		case 0:
			iShowImage(265, 218, "assets/buttons/settings_button.png");
			break;
		
		case 1:
			iShowImage(265, 218, "assets/buttons/settings_button_hover.png");
			break;

		case 2:
			iShowImage(265, 218, "assets/buttons/settings_button_pressed.png");
			break;
		default:
			break;
	}
}