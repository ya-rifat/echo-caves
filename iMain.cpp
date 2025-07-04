#include "iGraphics.h"
#include "iSound.h"
#include <string.h>
#include <stdbool.h>

#define SETTINGS_FILE "data/settings.dat"

//Images
Image reveal, cave_background;
Image lSide[30], front[30], back[30], rSide[30];
Sprite lsidev, frontv, backv, rsidev;

Sprite walk_front, walk_right, walk_left, walk_back, idle_front, idle_right, idle_left, idle_back;
Image walk_front_frames[8], walk_right_frames[8], walk_left_frames[8], walk_back_frames[8], idle_front_frames[8], idle_right_frames[8], idle_left_frames[8], idle_back_frames[8];
int character_direction = 0; //0 - left, 1 - right, 2 - front(facing the screen), 3 - back
bool is_moving = false; //detects character movement, if it's not moving play idle animation

int r = 0;

//function declarations
/* Screens */
void startScreen();
void storyScreen();
void underConstruction();
void iAnim();
void loadResources();
void game_screen();
void doFade();

/* Components */
void button(const char texture_name[], int pos_x, int pos_y, int width, int height, int *var_name, int var_value, int has_state = 1, int make_sound = 1);
void toggle(int pos_x, int pos_y, int width, int height, int *var_name, int var_value, int make_sound = 1);

//mouse actions
int mouse_x = 0, mouse_y = 0;
bool is_state_down = false, is_state_up = false, is_left_button = false;

bool button_click; //button click detector

//keuboard actions
unsigned char n_key, s_key; //normal_keyboard, special_keyboard
//currently testing

//screen tracker
int current_screen = 0;

//for sound control
bool button_sound;

//TODO: test
int reveal_x = -810, reveal_y = -430; //position

void iDraw() {
	iClear();

	switch (current_screen) {
		case 0:
			startScreen();
			break;
		case 4:
			storyScreen();
			break;

    case 10:
      game_screen();
      break;

		case 100:
			underConstruction();
			break;
		default:
			break;
	}

  if (isSpecialKeyPressed(GLUT_KEY_UP) || isSpecialKeyPressed(GLUT_KEY_DOWN) || isSpecialKeyPressed(GLUT_KEY_LEFT) || isSpecialKeyPressed(GLUT_KEY_RIGHT)) is_moving = true;
  else is_moving = false;
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
  
  if (!button_click && state == GLUT_UP) { //reset button states
    is_state_down = false;
    is_state_up = false;
    is_left_button = false;
  }
}

/*
 * dir = 1 for up, -1 for down.
 */
void iMouseWheel(int dir, int mx, int my) {}

void iKeyboard(unsigned char key) {
  n_key = key;

	switch (key) {
    case 'q':
			current_screen = 0;
			break;
		case ' ':
      if(current_screen == 10)
        r = 1;
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

void iSpecialKeyboard(unsigned char key) {
  s_key = key;

  switch (key) {
    case GLUT_KEY_UP:
      is_moving = true;
      if(current_screen == 10){
        if(character_direction != 3)
          character_direction = 3;
        else{
          reveal_y+=4;
        }
      }
      break;

    case GLUT_KEY_DOWN:
      is_moving = true;
      if(current_screen == 10)
      {
        if(character_direction != 2)
          character_direction = 2;
        else{
          reveal_y-=4;
        }
      }
      break;

    case GLUT_KEY_RIGHT:
      is_moving = true;
      if(current_screen == 10){
        if(character_direction != 1)
          character_direction = 1;
        else{
          reveal_x+=4;
        }
      }
      break;

    case GLUT_KEY_LEFT:
      is_moving = true;
      if(current_screen == 10)
      {
        if(character_direction != 0)
          character_direction = 0;
        else{
          reveal_x-=4;
        }
      }
      break;
    
    default:
      break;
  }
}

int main(int argc, char *argv[]) {
	glutInit(&argc, argv);
  iSetTimer(5000, doFade);
  iSetTimer(100, iAnim);

  //Images are to be loaded only once
  iLoadImage(&reveal, "assets/game_screen/test.png");
  iLoadImage(&cave_background, "assets/game_screen/4.png");
  loadResources();
	iInitializeSound();
	iInitialize(900, 600, "Echo Caves");

	return 0;
}

/*
 * Define screens here..........
 * 
 * Declare them on top first
 */

void startScreen() { //index 0
	// iShowImage(0,0, "assets/backgrounds/game_bg.jpg");
	iShowImage(787, 5, "assets/texts/version.png");

  button("play_button", 595, 330, 100, 100, &current_screen, 10);
  button("settings_button", 510, 234, 270, 60, &current_screen, 100);
  button("leaderboard_button", 510, 172, 270, 60, &current_screen, 100);
  button("story_button", 510, 110, 270, 60, &current_screen, 4);
  toggle(100, 100, 29, 16, &current_screen, 0);

  button("help_button", 80, 10, 60, 60, &current_screen, 0);
  int is_exit_pressed;
  button("exit_button", 10, 10, 60, 60, &is_exit_pressed, 1);
  if (is_exit_pressed) exit(0);
}

//the most important screen
/*
 * HIGHLIGHTED * 
 */

void game_screen() {
  if(r == 1)
    iShowLoadedImage(0, 0, &cave_background);

  switch (character_direction * 2 + is_moving) {
    case 0:
      iSetSpritePosition(&idle_left, reveal_x + 818, reveal_y + 558);
      iShowSprite(&idle_left);
      break;
    case 1:
      iSetSpritePosition(&walk_left, reveal_x + 818, reveal_y + 558);
      iShowSprite(&walk_left);
      break;
    case 2:
      iSetSpritePosition(&idle_right, reveal_x + 818, reveal_y + 558);
      iShowSprite(&idle_right);
      break;
    case 3:
      iSetSpritePosition(&walk_right, reveal_x + 818, reveal_y + 558);
      iShowSprite(&walk_right);
      break;
    case 4:
      iSetSpritePosition(&idle_front, reveal_x + 818, reveal_y + 558);
      iShowSprite(&idle_front);
      break;
    case 5:
      iSetSpritePosition(&walk_front, reveal_x + 818, reveal_y + 558);
      iShowSprite(&walk_front);
      break;
    case 6:
      iSetSpritePosition(&idle_back, reveal_x + 818, reveal_y + 558);
      iShowSprite(&idle_back);
      break;
    case 7:
      iSetSpritePosition(&walk_back, reveal_x + 818, reveal_y + 558);
      iShowSprite(&walk_back);
      break;
    default:
      break;
  }
  
  iShowLoadedImage(reveal_x, reveal_y, &reveal);
}

void storyScreen() { //index 4
	iShowImage(0, 0, "assets/backgrounds/story_bg.png");
	iShowImage(98, 200, "assets/texts/story.png");
  button("back", 390, 70, 102, 25, &current_screen, 0, 0);
}

void settings_screen() {
  
}

void underConstruction() { //screen index 100
	iText(10, 10, "Under Construction...   Come back later (Press q to go back)");
}

/*
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

  int state = 0;
  char texture_path[100] = "assets/buttons/";
  strcat(texture_path, texture_name);

  if ((mouse_x > pos_x && mouse_x < (pos_x + width)) && (mouse_y > pos_y && mouse_y < (pos_y + height))) {
    state = 1;

    if (is_left_button) {
      if (is_state_down) {
        state = 2;
        button_click = true;
        if (!button_sound && make_sound) {
          iPlaySound("assets/sounds/button_click.wav", false, 80);
          button_sound = true;
        }
      } else if (is_state_up) {
        *var_name = var_value;
        button_sound =  false;
        button_click = false;

        is_state_up = false;
        is_state_down = false;
        is_left_button = false;
      }
    }
  }

  switch (state) {
    case 0:
      strcat(texture_path, ".png");
      iShowImage(pos_x, pos_y, texture_path);
      break;
    case 1:
      strcat(texture_path, has_state ? "_hover.png" : ".png");
      iShowImage(pos_x, pos_y, texture_path);
      break;
    case 2:
      strcat(texture_path, has_state ? "_pressed.png" : ".png");
      iShowImage(pos_x, pos_y, texture_path);
      break;
    default:
      break;
  }
}

void toggle(int pos_x, int pos_y, int width, int height, int *var_name, int var_value, int make_sound) { //max length of texture path is 200
  
  /* Notes before using
   * Kind of same as button
   */

  int state = 0; //0, 2 initialization
  char texture_path[100] = "assets/buttons/toggle_";

  if ((mouse_x > pos_x && mouse_x < (pos_x + width)) && (mouse_y > pos_y && mouse_y < (pos_y + height))) {
    state = (state / 2) + 1;

    if (is_left_button) {
      if (is_state_down) {
        state = state + 1;
        button_click = true;
        if (!button_sound && make_sound) {
          iPlaySound("assets/sounds/button_click.wav", false, 80);
          button_sound = true;
        }
      } else if (is_state_up) {
        *var_name = var_value;
        button_sound =  false;
        button_click = false;

        is_state_up = false;
        is_state_down = false;
        is_left_button = false;
      }
    }
  } else state = 0;

  switch (state) {
    case 0: //checked
      strcat(texture_path, "checked.png");
      iShowImage(pos_x, pos_y, texture_path);
      break;
    case 1: //checked_hover
      strcat(texture_path, "checked_hover.png");
      iShowImage(pos_x, pos_y, texture_path);
      break;
    case 2: //unchecked
      strcat(texture_path, "unchecked.png");
      iShowImage(pos_x, pos_y, texture_path);
      break;
    case 3: //unchecked_hover
      strcat(texture_path, "unchecked_hover.png");
      iShowImage(pos_x, pos_y, texture_path);
      break;
    default:
      break;
  }
}

void doFade()
{
  if(r == 1)
    r = 0;
}

void loadResources() {
	iInitSprite(&idle_left);
  iInitSprite(&idle_right);
  iInitSprite(&idle_front);
  iInitSprite(&idle_back);
	iInitSprite(&walk_left);
  iInitSprite(&walk_right);
  iInitSprite(&walk_front);
  iInitSprite(&walk_back);
  iLoadFramesFromSheet(idle_left_frames, "assets/game_screen/character_spritesheets/idle_left.png", 1, 8);
  iLoadFramesFromSheet(idle_right_frames, "assets/game_screen/character_spritesheets/idle_right.png", 1, 8);
  iLoadFramesFromSheet(idle_front_frames, "assets/game_screen/character_spritesheets/idle_front.png", 1, 8);
  iLoadFramesFromSheet(idle_back_frames, "assets/game_screen/character_spritesheets/idle_back.png", 1, 8);
  iLoadFramesFromSheet(walk_left_frames, "assets/game_screen/character_spritesheets/walk_left.png", 1, 8);
  iLoadFramesFromSheet(walk_right_frames, "assets/game_screen/character_spritesheets/walk_right.png", 1, 8);
  iLoadFramesFromSheet(walk_front_frames, "assets/game_screen/character_spritesheets/walk_front.png", 1, 8);
  iLoadFramesFromSheet(walk_back_frames, "assets/game_screen/character_spritesheets/walk_back.png", 1, 8);
	iChangeSpriteFrames(&idle_left, idle_left_frames, 8);
  iChangeSpriteFrames(&idle_right, idle_right_frames, 8);
  iChangeSpriteFrames(&idle_front, idle_front_frames, 8);
  iChangeSpriteFrames(&idle_back, idle_back_frames, 8);
  iChangeSpriteFrames(&walk_left, walk_left_frames, 8);
  iChangeSpriteFrames(&walk_right, walk_right_frames, 8);
  iChangeSpriteFrames(&walk_front, walk_front_frames, 8);
  iChangeSpriteFrames(&walk_back, walk_back_frames, 8);
}

void iAnim() {
  switch (character_direction * 2 + is_moving) {
    case 0:
      iAnimateSprite(&idle_left);
      break;
    case 1:
      iAnimateSprite(&walk_left);
      break;
    case 2:
      iAnimateSprite(&idle_right);
      break;
    case 3:
      iAnimateSprite(&walk_right);
      break;
    case 4:
      iAnimateSprite(&idle_front);
      break;
    case 5:
      iAnimateSprite(&walk_front);
      break;
    case 6:
      iAnimateSprite(&idle_back);
      break;
    case 7:
      iAnimateSprite(&walk_back);
      break;
    default:
      break;
  }
}

/*
 * Read and Write Functions
 **************************
 * enum - destination_file: SETTINGS_FILE, ...
 * enum - type_of_data: toggle_state, ...
 * string - id: any name
 * pointer - value1, value2, ...
 * 
 * 
 */

//structures
typedef struct { //toggle state (0 - checked, 1 - unchecked)
    char id[20]; //name(id) of the data stored in the file
    int state;
} ToggleData; //- used for storing toggle data

void writeData(const char destination_file[], const char type_of_data[], const char id[], void *value1) {
  FILE *pFile = NULL;
  pFile = fopen(destination_file, "wb");
  if (pFile == NULL) {
    printf("Error opening file for reading/writing"); //todo
    exit(1);
  }
}

/*
 * Other necessary functions (currently none)
 */