#include "iGraphics.h"
#include "iSound.h"

//function declarations
void startScreen();
void storyScreen();
void underConstruction();

//button states
//start screen - 0
int play_button, settings_button, leaderboard_button, exit_button, help_button, story_button;

//story screen - 4
int back_button_story;

int current_screen; //0 means star screen

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
			if ((mx > 100 && mx < 370) && (my > 304 && my < 364)) settings_button = 1; else settings_button = 0;
			if ((mx > 100 && mx < 370) && (my > 242 && my < 302)) leaderboard_button = 1; else leaderboard_button = 0;
			if ((mx > 100 && mx < 370) && (my > 180 && my < 240)) story_button = 1; else story_button = 0;
			if ((mx > 830 && mx < 890) && (my > 10 && my < 70)) exit_button = 1; else exit_button = 0;
			if ((mx > 765 && mx < 825) && (my > 10 && my < 70)) help_button = 1; else help_button = 0;
			break;

		case 4:
			if ((mx > 395 && mx < 497) && (my > 75 && my < 100)) back_button_story = 1; else back_button_story = 0;
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
					if (play_button) play_button = 2;
					if (story_button) story_button = 2;
					if (settings_button) settings_button = 2;
					if (leaderboard_button) leaderboard_button = 2;
					if (exit_button) exit_button = 2;
					if (help_button) help_button = 2;
					break;

				case 4:
					if (back_button_story) back_button_story = 2;
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
					if (play_button) {
						play_button = 0;
						button_sound = 0;
					}
					if (settings_button) {
						current_screen = 100;
						settings_button = 0;
						button_sound = 0;
					}
					if (leaderboard_button) {
						current_screen = 100;
						leaderboard_button = 0;
						button_sound = 0;
					}
					if (story_button) {
						current_screen = 4;
						story_button = 0;
						button_sound = 0;
					}
					if (exit_button) exit(0);
					if (help_button) {
						current_screen = 0;
						help_button = 0;
						button_sound = 0;
					}
					
					break;

				case 4:
					if (back_button_story) {
						current_screen = 0;
						back_button_story = 0;
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
	iShowImage(218, 475, "assets/logo-2.png");
	iShowImage(10, 10, "assets/texts/version.png");

	// switch (play_button) {
	// 	case 0: iShowImage(100, 304, "assets/buttons/play_button.png"); break;
	// 	case 1: iShowImage(100, 304, "assets/buttons/play_button_hover.png"); break;
	// 	case 2: iShowImage(100, 304, "assets/buttons/play_button_pressed.png");
	// 		if (!button_sound) iPlaySound("assets/sounds/button_click.wav", false, 80);
	// 		button_sound = 1;
	// 		break;
	// }

	switch (settings_button) {
		case 0: iShowImage(100, 304, "assets/buttons/settings_button.png"); break;
		case 1: iShowImage(100, 304, "assets/buttons/settings_button_hover.png"); break;
		case 2: iShowImage(100, 304, "assets/buttons/settings_button_pressed.png");
			if (!button_sound) iPlaySound("assets/sounds/button_click.wav", false, 80);
			button_sound = 1;
			break;
	}

	switch (leaderboard_button) {
		case 0: iShowImage(100, 242, "assets/buttons/leaderboard_button.png"); break;
		case 1: iShowImage(100, 242, "assets/buttons/leaderboard_button_hover.png"); break;
		case 2: iShowImage(100, 242, "assets/buttons/leaderboard_button_pressed.png");
			if (!button_sound) iPlaySound("assets/sounds/button_click.wav", false, 80);
			button_sound = 1;
			break;
	}

	switch (story_button) {
		case 0: iShowImage(100, 180, "assets/buttons/story_button.png"); break;
		case 1: iShowImage(100, 180, "assets/buttons/story_button_hover.png"); break;
		case 2: iShowImage(100, 180, "assets/buttons/story_button_pressed.png");
			if (!button_sound) iPlaySound("assets/sounds/button_click.wav", false, 80);
			button_sound = 1;
			break;
	}

	switch (exit_button) {
		case 0: iShowImage(830, 10, "assets/buttons/exit_button.png"); break;
		case 1: iShowImage(830, 10, "assets/buttons/exit_button_hover.png"); break;
		case 2: iShowImage(830, 10, "assets/buttons/exit_button_pressed.png");
			if (!button_sound) iPlaySound("assets/sounds/button_click.wav", false, 80);
			button_sound = 1;
			break;
	}
	
	switch (help_button) {
		case 0: iShowImage(765, 10, "assets/buttons/help_button.png"); break;
		case 1: iShowImage(765, 10, "assets/buttons/help_button_hover.png"); break;
		case 2: iShowImage(765, 10, "assets/buttons/help_button_pressed.png");
			if (!button_sound) iPlaySound("assets/sounds/button_click.wav", false, 80);
			button_sound = 1;
			break;
	}
}

void storyScreen() { //screen index 4
	iShowImage(0, 0, "assets/backgrounds/story_bg.png");
	switch (back_button_story) {
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