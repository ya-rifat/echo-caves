#include "iGraphics.h"
#include "iSound.h"
#include <string.h>

//function declarations
/* Screens */
void startScreen();
void storyScreen();
void underConstruction();

/* Components */
void button(const char texture_name[], int pos_x, int pos_y, int width, int height, int *var_name, int var_value, int has_state = 1, int make_sound = 1);

//mouse actions
int mouse_x = 0, mouse_y = 0;
int is_state_down = 0, is_state_up = 0, is_left_button = 0;

int button_click; //button click detector

//screen tracker
int current_screen;

//for sound control
int button_sound;

//trash
int int_trash;

void iDraw() {
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
}

//mouse drag and move
void iMouseDrag(int mx, int my) {}

void iMouse(int button, int state, int mx, int my)
{
  is_state_down = state == GLUT_DOWN;
  is_state_up = state == GLUT_UP;
  is_left_button = button == GLUT_LEFT_BUTTON;
  
  if (button_click == 0 && state == GLUT_UP) { //reset
    is_state_down = 0;
    is_state_up = 0;
    is_left_button = 0;
  }
}

/*
 * dir = 1 for up, -1 for down.
 */
void iMouseWheel(int dir, int mx, int my) {}

void iKeyboard(unsigned char key) {
	switch (key) {
    case 'q':
			current_screen = 0;
			break;
			
    default:
			break;
  }
}

/*
 * GLUT_KEY_F1, GLUT_KEY_F2, GLUT_KEY_F3, GLUT_KEY_F4, GLUT_KEY_F5, GLUT_KEY_F6,
 * GLUT_KEY_F7, GLUT_KEY_F8, GLUT_KEY_F9, GLUT_KEY_F10, GLUT_KEY_F11,
 * GLUT_KEY_F12, GLUT_KEY_LEFT, GLUT_KEY_UP, GLUT_KEY_RIGHT, GLUT_KEY_DOWN,
 * GLUT_KEY_PAGE_UP, GLUT_KEY_PAGE_DOWN, GLUT_KEY_HOME, GLUT_KEY_END,
 * GLUT_KEY_INSERT
 */
void iSpecialKeyboard(unsigned char key) {}

int main(int argc, char *argv[]) {
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
// Image backgroundImage;
// iloadImage(&backgroundImage, "assets")
void startScreen() { //index 0
	iShowImage(0,0, "assets/backgrounds/GAME_BG.jpg");
	iShowImage(5, 5, "assets/texts/version.png");

  button("play_button", 610, 320, 100, 100, &current_screen, 100);
  button("settings_button", 530, 254, 270, 60, &current_screen, 100);
  button("leaderboard_button", 530, 192, 270, 60, &current_screen, 100);
  button("story_button", 530, 130, 270, 60, &current_screen, 4);
  button("help_button", 765, 10, 60, 60, &current_screen, 0);

  int is_exit_pressed;
  button("exit_button", 830, 10, 60, 60, &is_exit_pressed, 1);
  if (is_exit_pressed) exit(0);
}

//the most important screen
/*** HIGHLIGHTED ***/
void game_screen() {
	
}

void storyScreen() { //index 4
	iShowImage(0, 0, "assets/backgrounds/story_bg.png");
  	button("back", 390, 70, 102, 25, &current_screen, 0, 0);
	iShowImage(98, 200, "assets/texts/story.png");
}


void underConstruction() { //screen index 100
	iText(10, 10, "Under Construction...   Come back later (Press q to go back)");
}

/**
 * components
 */

void button(const char texture_name[], int pos_x, int pos_y, int width, int height, int *var_name, int var_value, int has_state, int make_sound) { //max length of texture path is 200
  
  /* Notes before using
   * Button texture must be in assets/buttons folder
   * if button does not have hover and pressed state set has_state parameter to 0, e.g back button in story_screen
   * if button name is given "abc" for example,
   * default texture name must be abc.png
   * hover texture name must be abc_hover.png
   * pressed texture name must be abc_pressed.png
   * put texture width and height in width and height parameter
   * if you do not want button to make sound set make_sound param to 0
   */

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
        button_click = 0;

        is_state_up = 0;
        is_state_down = 0;
        is_left_button = 0;
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
      strcat(texture, has_state ? "_hover.png" : ".png");
      iShowImage(pos_x, pos_y, texture);
      break;
    case 2:
      strcpy(texture, path);
      strcat(texture, has_state ? "_pressed.png" : ".png");
      iShowImage(pos_x, pos_y, texture);
			if (!button_sound && make_sound) {
        iPlaySound("assets/sounds/button_click.wav", false, 80);
        button_sound = 1;
      }
      break;
    default:
      break;
  }
}