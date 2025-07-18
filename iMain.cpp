#include "iGraphics.h"
#include "iSound.h"
#include "iFont.h"
#include <string.h>
#include <stdbool.h>

#define CUSTOM_FONT "assets/fonts/Minecraft-Five.ttf"
#define SETTINGS_FILE "data/settings.dat"

#define PLAYER_SPEED 5
#define UPPER_BOUND 500
#define MAXENTRIES 50
//function declarations
/* Screens */
void startScreen();
void storyScreen();
void underConstruction();
void game_screen();
void updateLeaderboard(char []);
void showLeaderboard();
void prompt_screen();
void settings_screen();

/* Extra */
void animate_player();
void loadResources();
void doFade();
void move_player(int direction);
void countTime();
void showTime();

/* Components */
void button(const char texture_name[], int pos_x, int pos_y, int width, int height, int *var_name, int var_value, int has_state = 1, int make_sound = 1);
void toggle(int pos_x, int pos_y, int width, int height, int *var_name, int var_value, int make_sound = 1);

// time count korar jonno variable
int isGameRunning, hours, mins, sec, secPassed = 0;
//player er name neyar jonno
int isNameGiven = 0;
int nameLength = 0;
char playerName[100];
// music 
int music_setting = 1;
int bgm;
//Images
Image reveal, cave_background;
Image walk_front_frames[8], walk_right_frames[8], walk_left_frames[8], walk_back_frames[8], idle_front_frames[8], idle_right_frames[8], idle_left_frames[8], idle_back_frames[8];
Sprite walk_front, walk_right, walk_left, walk_back, idle_front, idle_right, idle_left, idle_back;
Sprite player_hitbox, cave_collision_box;
Image player_hitbox_image, cave_collision_box_image;
int character_direction = 1; //0 - left, 1 - right, 2 - front(facing the screen), 3 - back
bool is_moving = false; //to detect character movement, if it's not moving play idle animation
bool is_background_moving = false; //background moving

int r = 0;

//mouse actions
int mouse_x = 0, mouse_y = 0;
bool is_state_down = false, is_state_up = false, is_left_button = false;

bool button_click; //button click detector

//screen tracker
int current_screen = 0;

//for sound control
bool button_sound;

//player position
int position_x = 70, position_y = 344; //todo: initialize with saved position
int background_position_x = -55, background_position_y = -600;

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
    case 20:
      prompt_screen();
      break;
    case 30:
      showLeaderboard();
      break;
    case 40:
      settings_screen();
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

void iMouse(int button, int state, int mx, int my) {
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

void iKeyPress(unsigned char key) {
  if (current_screen == 20 && isNameGiven == 0)
  {
    if (key == '\r')
    {
      if (nameLength > 0)
      {
        isNameGiven = 1;
      }
    }
    else if (key == '\b')
    {
      if (nameLength > 0)
      {
        nameLength--;
        playerName[nameLength] = '\0';
      }
    }
    else if (nameLength < 99 && key >= 32 && key <= 126)
    {
      playerName[nameLength++] = key;
      playerName[nameLength] = '\0';
    }
  }
  else
  {
    switch (key)
    {
    case 'q':
      if (current_screen == 10)
      {
        isGameRunning = 0;
        updateLeaderboard(playerName);
        secPassed = 0;
        if(music_setting != 0)
          iIncreaseVolume(bgm, 100);
      }
      current_screen = 0;
      break;
    case ' ':
      if (current_screen == 10){
        r = 1;
        iPlaySound("assets/sounds/echo.wav",0,100);
      }
      break;
    default:
      break;
    }
  }
}

/*
 * GLUT_KEY_F1, GLUT_KEY_F2, GLUT_KEY_F3, GLUT_KEY_F4, GLUT_KEY_F5, GLUT_KEY_F6,
 * GLUT_KEY_F7, GLUT_KEY_F8, GLUT_KEY_F9, GLUT_KEY_F10, GLUT_KEY_F11,
 * GLUT_KEY_F12, GLUT_KEY_LEFT, GLUT_KEY_UP, GLUT_KEY_RIGHT, GLUT_KEY_DOWN,
 * GLUT_KEY_PAGE_UP, GLUT_KEY_PAGE_DOWN, GLUT_KEY_HOME, GLUT_KEY_END,
 * GLUT_KEY_INSERT
 */

void iSpecialKeyPress(unsigned char key) {
  switch (current_screen) {
    case 10:
      switch (key) {
        case GLUT_KEY_UP:
          character_direction = 3;
          move_player(character_direction);
          break;
        case GLUT_KEY_DOWN:
          character_direction = 2;
          move_player(character_direction);
          break;
        case GLUT_KEY_RIGHT:
          character_direction = 1;
          move_player(character_direction);
          break;
        case GLUT_KEY_LEFT:
          character_direction = 0;
          move_player(character_direction);
          break;
        default:
          break;
      }
      break;
    
    default:
      break;
  }
}

int main(int argc, char *argv[]) {
	glutInit(&argc, argv);
  iSetTimer(7000, doFade);
  iSetTimer(100, animate_player);
  iSetTimer(1000, countTime);

  //Images are to be loaded only once
  loadResources();
  bgm = iPlaySound("assets/sounds/Numb.wav", true, 60);
	iOpenWindow(900, 600, "Echo Caves");
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
   if(!isNameGiven)
    button("play_button", 595, 330, 100, 100, &current_screen, 20);
  else
    button("play_button", 595, 330, 100, 100, &current_screen, 10);
  button("settings_button", 510, 234, 270, 60, &current_screen, 40);
  button("leaderboard_button", 510, 172, 270, 60, &current_screen, 30);
  button("story_button", 510, 110, 270, 60, &current_screen, 4);

  button("help_button", 80, 10, 60, 60, &current_screen, 0);
  int is_exit_pressed;
  button("exit_button", 10, 10, 60, 60, &is_exit_pressed, 1);
  if (is_exit_pressed) iCloseWindow();
}

//the most important screen
/*
 * HIGHLIGHTED * 
 */

void game_screen() {
  iDecreaseVolume(bgm, 100);
  isGameRunning = 1;
  if(r == 1)
  iShowLoadedImage(background_position_x, background_position_y, &cave_background);

  if (isSpecialKeyPressed(GLUT_KEY_UP) || isSpecialKeyPressed(GLUT_KEY_DOWN) || isSpecialKeyPressed(GLUT_KEY_LEFT) || isSpecialKeyPressed(GLUT_KEY_RIGHT)) is_moving = true;
  else is_moving = false;

  if (position_x >= 100 && position_x <= 800 && position_y >= 100 && position_y <= 500)
    is_background_moving = false;
  else {
    is_background_moving = true;
  }

  switch (character_direction * 2 + is_moving) {
    case 0:
      iSetSpritePosition(&idle_left, position_x, position_y);
      iShowSprite(&idle_left);
      break;
    case 1:
      iSetSpritePosition(&walk_left, position_x, position_y);
      iShowSprite(&walk_left);
      break;
    case 2:
      iSetSpritePosition(&idle_right, position_x, position_y);
      iShowSprite(&idle_right);
      break;
    case 3:
      iSetSpritePosition(&walk_right, position_x, position_y);
      iShowSprite(&walk_right);
      break;
    case 4:
      iSetSpritePosition(&idle_front, position_x, position_y);
      iShowSprite(&idle_front);
      break;
    case 5:
      iSetSpritePosition(&walk_front, position_x, position_y);
      iShowSprite(&walk_front);
      break;
    case 6:
      iSetSpritePosition(&idle_back, position_x, position_y);
      iShowSprite(&idle_back);
      break;
    case 7:
      iSetSpritePosition(&walk_back, position_x, position_y);
      iShowSprite(&walk_back);
      break;
    default:
      break;
  }
  
  iShowLoadedImage(position_x - 870, position_y - 580, &reveal);
  showTime();
}

void storyScreen() { //index 4
	iShowImage(0, 0, "assets/backgrounds/story_bg.png");
	iShowImage(98, 200, "assets/texts/story.png");
  button("back", 390, 70, 102, 25, &current_screen, 0, 0);
}

void settings_screen() {
  int init_music_setting = music_setting;
  iSetColor(75, 54, 33);
  iShowImage(0,0,"assets/backgrounds/story_bg.png");
  toggle(500, 450, 64, 64, &music_setting, 1, 1);
  
  if(music_setting != init_music_setting){
    init_music_setting = music_setting;
    if(music_setting == 0)
      iDecreaseVolume(bgm, 100);
    else 
      iIncreaseVolume(bgm, 100);
  }
  char msg[50];
  sprintf(msg, "SOUND : %s", music_setting ? "ON" : "OFF");
  iShowText(370, 450, msg, CUSTOM_FONT, 18);
}

void underConstruction() { //screen index 100
	iShowText(10, 10, "Under Construction...   Come back later (Press q to go back)", CUSTOM_FONT);
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

// void toggle(int pos_x, int pos_y, int width, int height, int *var_name, int var_value, int make_sound) { //max length of texture path is 200
  
//   /* Notes before using
//    * Kind of same as button
//    */

//   int state = 0; //0, 2 initialization
//   char texture_path[100] = "assets/buttons/toggle_";

//   if ((mouse_x > pos_x && mouse_x < (pos_x + width)) && (mouse_y > pos_y && mouse_y < (pos_y + height))) {
//     state = (state / 2) + 1;

//     if (is_left_button) {
//       if (is_state_down) {
//         state = state + 1;
//         button_click = true;
//         if (!button_sound && make_sound) {
//           iPlaySound("assets/sounds/button_click.wav", false, 80);
//           button_sound = true;
//         }
//       } else if (is_state_up) {
//         *var_name = var_value;
//         button_sound =  false;
//         button_click = false;

//         is_state_up = false;
//         is_state_down = false;
//         is_left_button = false;
//       }
//     }
//   } else state = 0;

//   switch (state) {
//     case 0: //checked
//       strcat(texture_path, "checked.png");
//       iShowImage(pos_x, pos_y, texture_path);
//       break;
//     case 1: //checked_hover
//       strcat(texture_path, "checked_hover.png");
//       iShowImage(pos_x, pos_y, texture_path);
//       break;
//     case 2: //unchecked
//       strcat(texture_path, "unchecked.png");
//       iShowImage(pos_x, pos_y, texture_path);
//       break;
//     case 3: //unchecked_hover
//       strcat(texture_path, "unchecked_hover.png");
//       iShowImage(pos_x, pos_y, texture_path);
//       break;
//     default:
//       break;
//   }
// }
void toggle(int pos_x, int pos_y, int width, int height, int *var_name, int var_value, int make_sound) {
    char texture_path[100] = "assets/buttons/toggle_";
    static bool was_pressed = false;

    bool is_inside = (mouse_x >= pos_x && mouse_x <= pos_x + width && mouse_y >= pos_y && mouse_y <= pos_y + height);
    bool is_checked = (*var_name == var_value);

    int state = 0;
    if (is_checked) state = is_inside ? 1 : 0;
    else            state = is_inside ? 3 : 2;

    switch (state) {
        case 0: strcat(texture_path, "checked.png"); break;
        case 1: strcat(texture_path, "checked_hover.png"); break;
        case 2: strcat(texture_path, "unchecked.png"); break;
        case 3: strcat(texture_path, "unchecked_hover.png"); break;
    }

    iShowImage(pos_x, pos_y, texture_path);

    if (is_inside && is_left_button) {
        was_pressed = true;
    } else if (was_pressed) {
        if (is_inside) {
            *var_name = is_checked ? 0 : var_value;
            if (make_sound) {
                iPlaySound("assets/sounds/button_click.wav", false, 80);
            }
        }
        was_pressed = false;
    }
}
/*
 * Read and Write Functions
 **************************
 * enum - destination_file: SETTINGS_FILE, ...
 * enum - type_of_data: toggle_state, ...
 * string - id: any name
 * pointer - value1, value2, ...
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


void doFade() {
  if(r == 1)
    r = 0;
}

void loadResources() {
  iLoadImage(&reveal, "assets/game_screen/test.png");
  iLoadImage(&cave_background, "assets/game_screen/cave_background.png");
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
  iLoadImage(&player_hitbox_image, "assets/game_screen/character_spritesheets/player_hitbox.png");
  iChangeSpriteFrames(&player_hitbox, &player_hitbox_image, 1);
  iLoadImage(&cave_collision_box_image, "assets/game_screen/cave_collision_box.png");
  iChangeSpriteFrames(&cave_collision_box, &cave_collision_box_image, 1);
  iSetSpritePosition(&player_hitbox, position_x - 1, position_y - 4);
}

void animate_player() {
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

void move_player(int direction) {
  int tentative_x = position_x;
  int tentative_y = position_y;

  switch (direction) {
    case 0: tentative_x -= PLAYER_SPEED; break;
    case 1: tentative_x += PLAYER_SPEED; break;
    case 2: tentative_y -= PLAYER_SPEED; break;
    case 3: tentative_y += PLAYER_SPEED; break;
  }

  iSetSpritePosition(&player_hitbox, tentative_x - 1, tentative_y - 4);
  if (!iCheckCollision(&cave_collision_box, &player_hitbox)) {
    position_x = tentative_x;
    position_y = tentative_y;
  }

  if (position_x < 100) {
    position_x = 100;
    background_position_x += PLAYER_SPEED;
  } else if (position_x > 800) {
    position_x = 800;
    background_position_x -= PLAYER_SPEED;
  }

  if (position_y < 80) {
    position_y = 80;
    background_position_y += PLAYER_SPEED;
  } else if (position_y > 480) {
    position_y = 480;
    background_position_y -= PLAYER_SPEED;
  }

  iSetSpritePosition(&player_hitbox, position_x - 1, position_y - 4);
  iSetSpritePosition(&cave_collision_box, background_position_x, background_position_y);
}

void countTime() {
  if(isGameRunning == 1)
    secPassed++;
}

void showTime() {
  if (isGameRunning) {
        mins = secPassed / 60;
        sec = secPassed % 60;
        hours = mins/60;
        char timeText[20];
        sprintf(timeText, "%02d:%02d:%02d", hours, mins, sec);
        iSetColor(255,255,0);
        iShowText(800, 580, timeText, CUSTOM_FONT, 18);
    }
}

typedef struct {
  char name[100];
  int seconds; // eita lagbe karon amra chai leaderboard sort korte
  char time[10];
} player;

// ekn player structure er ekta array lagbe! 
player leaderboard[MAXENTRIES];

void prompt_screen() {
  iClear();
  iSetColor(75, 54, 33);
  iShowImage(0,0,"assets/backgrounds/story_bg.png");
  if (!isNameGiven) {
        iShowText(100, 400, "ENTER YOUR NAME : ", CUSTOM_FONT, 18);
        iShowText(290, 400, playerName, CUSTOM_FONT, 18);
        iShowText(100, 300, "Press ENTER to start", CUSTOM_FONT, 18);
    } else {
        current_screen = 10;
    }
}

void showLeaderboard() {
    iShowImage(0,0,"assets/backgrounds/story_bg.png");
    FILE *fp = fopen("data/leaderboard.dat", "rb");
    int cnt = 0;
    if (fp != NULL) {
        char name[100], time[100];
        while (fscanf(fp, "%s %s", name, time) == 2) {
            if(cnt >= MAXENTRIES) break;
            // copy korte hobe
            strcpy(leaderboard[cnt].name, name);
            strcpy(leaderboard[cnt].time, time);
            int hr, mm, ss;
            sscanf(time, "%d:%d:%d", &hr, &mm, &ss);
            leaderboard[cnt].seconds = hr * 3600 + mm * 60 + ss;
            cnt++;
        }
        fclose(fp);
    } else {
        printf("Error opening leaderboard file.\n");
    }

    // ekhn sort kore felte hobe
    for (int i = 0; i < cnt - 1; i++) {
        for (int j = i + 1; j < cnt; j++) {
            if (leaderboard[j].seconds < leaderboard[i].seconds) {
                player temp = leaderboard[i];
                leaderboard[i] = leaderboard[j];
                leaderboard[j] = temp;
            }
        }
    }
    int top = cnt < 5 ? cnt : 5;
    // ekn amar kaj top 10 store kora
    fp = NULL;
    fp = fopen("data/leaderboard.dat", "wb");
    if(fp!=NULL)
    {
      for(int i = 0; i < top; i++){
         fprintf(fp, "%s %s\n", leaderboard[i].name, leaderboard[i].time);
      }
      fclose(fp);
    }
    else{
      printf("Error opening the file\n");
      exit(1);
    }
    //leaderboard ekn show korte hobe
    iSetColor(75, 54, 33);
    int y = 500;
    iShowText(390, y + 30, "LEADERBOARD", CUSTOM_FONT, 24);
    for (int i = 0; i < cnt && i < 5; i++) {
        char entry[100];
        entry[149] = '\0';
        char time[10];
        sprintf(entry, "%02d. %s", i + 1, leaderboard[i].name);
        sprintf(time, "%s", leaderboard[i].time);
        iShowText(120, y, entry, CUSTOM_FONT, 18);
        iShowText(240, y, time, CUSTOM_FONT, 18);
        y -= 30;
    }
}

void updateLeaderboard(char playerName[]) {
  FILE *fp = NULL;
  fp = fopen("data/leaderboard.dat", "a");
  if(fp != NULL)
  {
    fprintf(fp, "%s %02d:%02d:%02d\n", playerName, hours, mins, sec);
    fclose(fp);
  }
  else{
    printf("Error opening the leaderboard file.\n");
    exit(1);
  }
}