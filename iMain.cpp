#include "iGraphics.h"
#include "iSound.h"

#define max_buttons 50 //in one screen

//function declarations
void startScreen();
void storyScreen();
void underConstruction();

//button states
int button_states[max_buttons] = {0}; //one array for all buttons states

//screen tracker
int current_screen;

//sound channels
//start screen
int button_sound;

void iDraw()
{
	iClear();

	switch (current_screen) {
		case 0:
			startScreen();
			break;

		case 4:
			storyScreen();
			break;
		
		case 100:
			underConstruction();
			break;

		default:
			break;
	}
}

//mouse move
void iMouseMove(int mx, int my) {
	switch (current_screen) {
		case 0:
			if ((mx > 600 && mx < 700) && (my > 205 && my < 305)) button_states[0] = 1; else button_states[0] = 0;
			if ((mx > 100 && mx < 370) && (my > 284 && my < 344)) button_states[1] = 1; else button_states[1] = 0;
			if ((mx > 100 && mx < 370) && (my > 222 && my < 282)) button_states[2] = 1; else button_states[2] = 0;
			if ((mx > 100 && mx < 370) && (my > 160 && my < 220)) button_states[3] = 1; else button_states[3] = 0;
			if ((mx > 765 && mx < 825) && (my > 10 && my < 70)) button_states[4] = 1; else button_states[4] = 0;
			if ((mx > 830 && mx < 890) && (my > 10 && my < 70)) button_states[5] = 1; else button_states[5] = 0;
			break;

		case 4:
			if ((mx > 395 && mx < 497) && (my > 75 && my < 100)) button_states[0] = 1; else button_states[0] = 0;
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
			switch (current_screen) {
				case 0:
					if (button_states[0]) button_states[0] = 2;
					if (button_states[1]) button_states[1] = 2;
					if (button_states[2]) button_states[2] = 2;
					if (button_states[3]) button_states[3] = 2;
					if (button_states[4]) button_states[4] = 2;
					if (button_states[5]) button_states[5] = 2;
					break;

				case 4:
					if (button_states[0]) button_states[0] = 2;
					break;

				case 100:
					break;
				
				default:
					break;
			}
    }
		if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
			switch (current_screen) {
				case 0:
					if (button_states[0]) {
						button_states[0] = 0;
						button_sound = 0;
					}
					if (button_states[1]) {
						current_screen = 100;
						button_states[1] = 0;
						button_sound = 0;
					}
					if (button_states[2]) {
						current_screen = 100;
						button_states[2] = 0;
						button_sound = 0;
					}
					if (button_states[3]) {
						current_screen = 4;
						button_states[3] = 0;
						button_sound = 0;
					}
					if (button_states[4]) {
						current_screen = 0;
						button_states[4] = 0;
						button_sound = 0;
					}
					if (button_states[5]) exit(0);
					
					break;

				case 4:
					if (button_states[0]) {
						current_screen = 0;
						button_states[0] = 0;
						button_sound = 0;
					}
					break;

				case 100:
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
			current_screen = 0;
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
	current_screen = 0;
	iInitializeSound();
	iInitialize(900, 600, "Echo Caves");
	return 0;
}



/**
 * Define screens here..........
 * 
 * Declare them on top first
 */


void startScreen() { //screen index 0
	iShowImage(182, 450, "assets/logo.png");
	iShowImage(10, 10, "assets/texts/version.png");

	switch (button_states[0]) {
		case 0: iShowImage(600, 205, "assets/buttons/play_button.png"); break;
		case 1: iShowImage(600, 205, "assets/buttons/play_button_hover.png"); break;
		case 2: iShowImage(600, 205, "assets/buttons/play_button_pressed.png");
			if (!button_sound) iPlaySound("assets/sounds/button_click.wav", false, 80);
			button_sound = 1;
			break;
	}

	switch (button_states[1]) {
		case 0: iShowImage(100, 284, "assets/buttons/settings_button.png"); break;
		case 1: iShowImage(100, 284, "assets/buttons/settings_button_hover.png"); break;
		case 2: iShowImage(100, 284, "assets/buttons/settings_button_pressed.png");
			if (!button_sound) iPlaySound("assets/sounds/button_click.wav", false, 80);
			button_sound = 1;
			break;
	}

	switch (button_states[2]) {
		case 0: iShowImage(100, 222, "assets/buttons/leaderboard_button.png"); break;
		case 1: iShowImage(100, 222, "assets/buttons/leaderboard_button_hover.png"); break;
		case 2: iShowImage(100, 222, "assets/buttons/leaderboard_button_pressed.png");
			if (!button_sound) iPlaySound("assets/sounds/button_click.wav", false, 80);
			button_sound = 1;
			break;
	}

	switch (button_states[3]) {
		case 0: iShowImage(100, 160, "assets/buttons/story_button.png"); break;
		case 1: iShowImage(100, 160, "assets/buttons/story_button_hover.png"); break;
		case 2: iShowImage(100, 160, "assets/buttons/story_button_pressed.png");
			if (!button_sound) iPlaySound("assets/sounds/button_click.wav", false, 80);
			button_sound = 1;
			break;
	}
	
	switch (button_states[4]) {
		case 0: iShowImage(765, 10, "assets/buttons/help_button.png"); break;
		case 1: iShowImage(765, 10, "assets/buttons/help_button_hover.png"); break;
		case 2: iShowImage(765, 10, "assets/buttons/help_button_pressed.png");
			if (!button_sound) iPlaySound("assets/sounds/button_click.wav", false, 80);
			button_sound = 1;
			break;
	}

	switch (button_states[5]) {
		case 0: iShowImage(830, 10, "assets/buttons/exit_button.png"); break;
		case 1: iShowImage(830, 10, "assets/buttons/exit_button_hover.png"); break;
		case 2: iShowImage(830, 10, "assets/buttons/exit_button_pressed.png");
			if (!button_sound) iPlaySound("assets/sounds/button_click.wav", false, 80);
			button_sound = 1;
			break;
	}
}

void storyScreen() { //screen index 4
	iShowImage(0, 0, "assets/backgrounds/story_bg.png");
	switch (button_states[0]) {
		case 0:
		case 1: iShowImage(400, 80, "assets/texts/back.png"); break;
		case 2: iShowImage(400, 80, "assets/texts/back.png");
			if (!button_sound) iPlaySound("assets/sounds/button_click.wav", false, 80);
			button_sound = 1;
			break;
	}
	iShowImage(98, 200, "assets/texts/story.png");
}

void underConstruction() { //screen index 100
	iText(10, 10, "Under Construction...   Come back later (Press q to go back)");
}