#include "iGraphics.h"
#include "iSound.h"
#include <string.h>

#define max_buttons 50 //in one screen

//function declarations
void startScreen();
void storyScreen();
void underConstruction();
void button(const char texture_name[], int pos_x, int pos_y, int width, int height, int *var_name, int var_value);

//mouse events
int mouse_x = 0, mouse_y = 0;
int is_state_down = 0, is_state_up = 0, is_left_button = 0;
int button_click; //button click detector

//button states
int button_states[max_buttons] = {0}; //one array for all buttons states

//screen tracker
int current_screen;

//sound channels
//start screen
int button_sound;

//trash
int int_trash;

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
  mouse_x = mx;
  mouse_y = my;
	switch (current_screen) {
		case 0:
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
  is_state_down = state == GLUT_DOWN;
  is_state_up = state == GLUT_UP;
  is_left_button = button == GLUT_LEFT_BUTTON;
  
  if (button_click == 0 && state == GLUT_UP) {
    is_state_down = 0;
    is_state_up = 0;
    is_left_button = 0;
  }
  
  
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
			switch (current_screen) {
				case 0:
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

  button("play_button", 600, 205, 100, 100, &current_screen, 100);
  button("settings_button", 100, 284, 270, 60, &current_screen, 100);
  button("leaderboard_button", 100, 222, 270, 60, &current_screen, 100);
  button("story_button", 100, 160, 270, 60, &current_screen, 4);
  button("help_button", 765, 10, 60, 60, &current_screen, 0);

  int is_exit_pressed;
  button("exit_button", 830, 10, 60, 60, &is_exit_pressed, 1);
  if (is_exit_pressed) exit(0);
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


void game_screen() {

}

void underConstruction() { //screen index 100
	iText(10, 10, "Under Construction...   Come back later (Press q to go back)");
}

/**
 * components
 */

void button(const char texture_name[], int pos_x, int pos_y, int width, int height, int *var_name, int var_value) { //max length of texture path is 200
  int state;
  char path[200] = "assets/buttons/";
  strcat(path, texture_name);
  char texture[200];

  if ((mouse_x > pos_x && mouse_x < (pos_x + width)) && (mouse_y > pos_y && mouse_y < (pos_y + height))) {
    state = 1;

    if (is_left_button) {
      if (is_state_down) {
        state = 2;
        button_click = 1;
      } else if (is_state_up) {
        *var_name = var_value;
        button_sound =  0;

        is_state_up = 0;
        is_state_down = 0;
        is_left_button = 0;
        button_click = 0;
      }
    }
  } else state = 0;

  switch (state) {
    case 0:
      strcpy(texture, path);
      strcat(texture, ".png");
      iShowImage(pos_x, pos_y, texture);
      break;

    case 1:
      strcpy(texture, path);
      strcat(texture, "_hover.png");
      iShowImage(pos_x, pos_y, texture);
      break;

    case 2:
      strcpy(texture, path);
      strcat(texture, "_pressed.png");
      iShowImage(pos_x, pos_y, texture);
			if (!button_sound) iPlaySound("assets/sounds/button_click.wav", false, 80);
			button_sound = 1;
      break;
    
    default:
      break;
  }
}