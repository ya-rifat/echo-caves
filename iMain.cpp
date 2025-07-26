#include "iGraphics.h"
#include "iSound.h"
#include "iFont.h"
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define CUSTOM_FONT "assets/fonts/Minecraft-Five.ttf"
#define LEADERBOARD_FILE "data/leaderboard.dat"
#define SETTINGS_FILE "data/settings.dat"
#define GAME_DATA "data/gamedata.dat"

#define PLAYER_SPEED 5
#define MAX_LEADERBOARD_DISPLAY 5
#define ENTRY_SIZE 100
#define ENTRY_ID_SIZE 32
#define MAX_WAYPOINTS 10
#define WAYPOINT_SPACING 50
#define MAX_STARS 5
float MONSTER_SPEED;
float LOS_STEP;

//enums
typedef enum {
  TYPE_INTEGER = 1,
  TYPE_STRING = 2,
  TYPE_FLOAT = 3,
  TYPE_VEC2 = 4,
  TYPE_LEADERBOARD = 5,
  TYPE_WAYPOINTS = 6
} FileDataType;

enum Screen {
  START = 0,
  SETTINGS = 1,
  LEADERBOARD = 2,
  INSTRUCTIONS = 3,
  STORY = 4,
  GAME_SCREEN = 5,
  NAME = 6,
  PLAY = 7,
  LOADING = 8,
  PAUSE = 9,
  GAME_OVER = 10,
  ERROR_SCREEN = 11,
  VICTORY = 12,
  LEVEL_UP = 13,
  UNDER_CONSTRUCTION = 100
};
int current_screen = START;

//structures
typedef struct {
  float x, y;
} Vec2;

typedef struct {
  int seconds;
  char time[10];
} LeaderboardData;

typedef struct {
  char id[ENTRY_ID_SIZE];
  union {
    int int_num;
    float float_num;
    char string[68];
    Vec2 vec2;
    LeaderboardData leaderboard;
    Vec2 waypoint[8];
  } value;
} FileData;

//function declarations
/* Screens */
void startScreen();
void storyScreen();
void underConstruction();
void game_screen();
void instructions_screen();
void showLeaderboard();
void prompt_screen();
void play_screen();
void pause_screen();
void game_over();
void victory_screen();
void error_screen();
void loading_screen();
void level_up();
void settings_screen();

void showTime();

/* Extra */
void save_game();
void read_saved_game();
void animate_stuff();
void loadResources();
void doFade();
void game_escape(int escape = PAUSE);
void reset_game();
bool spawn_monster();
void clear_file_data(const char file[]);
void move_player(int direction);
void offset_monster(float dx, float dy, int direction);
void monster_ai();
void update_waypoints();
void offset_waypoints(int offset, int direction);
bool check_los(Vec2 from, Vec2 to);
void countTime();
void updateLeaderboard();
void writeData(const char destination_file[], FileDataType type_of_data, const char id[], void *value1, void *value2 = nullptr);
FileData readData(const char source_file[], const char id[], bool is_array = false);

/* Components */
void button(const char texture_name[], int pos_x, int pos_y, int width, int height, int *var_name, int var_value, int has_state = 1, int make_sound = 1);
void toggle(const char toggle_name[], int pos_x, int pos_y, bool *var_name, int make_sound = 1);

//game control
int hours, mins, sec, secPassed = 0;
int loading_destination, prompt_source, level_init;
int current_level = 1, monster_state = 0, stars_collected = 0;
/* position */
Vec2 player_pos, background_pos, monster_pos = {200, 344}; //todo
Vec2 waypoints[MAX_WAYPOINTS];
float monster_offset;
Vec2 monster_target;
bool has_target = false;
int monster_idle_timer = 0;
int monster_despawn_timer = 0;
int animation_direction, draw_order;

int difficulty;

//player er name neyar jonno
int nameLength = 0;
char playerName[30], temp_player_name[30];
bool isNameGiven;

//timer ids
int reveal_timer, monster_update, animate_timer, loading_timer, time_count;

//sound control and channels
bool background_music_setting;
int background_music, running_sound;

//Images and Sprites
Image current_background;
Image reveal, cave_background, loading_image[10], player_pointer, monster_pointer, star_image;
Image player_hitbox_image, cave_collision_box_image, timer_background, los_checker_image, monster_hitbox_image;
Image walk_front_frames[8], walk_right_frames[8], walk_left_frames[8], walk_back_frames[8], idle_front_frames[8], idle_right_frames[8], idle_left_frames[8], idle_back_frames[8], anim_start_player[8];
Sprite walk_front, walk_right, walk_left, walk_back, idle_front, idle_right, idle_left, idle_back, anim_start;
Sprite player_hitbox, cave_collision_box, monster_hitbox, los_checker, loading;
int character_direction = 1; //0 - left, 1 - right, 2 - front(facing the screen), 3 - back
bool is_moving = false; int is_background_moving = 0; //to detect character movement, if it's not moving play idle animation
bool is_revealed = false;
//temporary
Image monster_temp_image, monster_idle_frames[6], monster_walk_right_frames[8], monster_walk_left_frames[8];
Sprite monster_temp, monster_idle, monster_walk_right, monster_walk_left;

//debug
Image waypoints_debug;

//mouse actions
int mouse_x = 0, mouse_y = 0;
bool is_state_down = false, is_state_up = false, is_left_button = false;
bool button_click; //button click detector

void iDraw() {
	iClear();

	switch (current_screen) {
		case START: startScreen(); break;
    case SETTINGS: settings_screen(); break;
    case LEADERBOARD: showLeaderboard(); break;
		case STORY: storyScreen(); break;
    case INSTRUCTIONS: instructions_screen(); break;
    case GAME_SCREEN: game_screen(); break;
    case NAME: prompt_screen(); break;
    case PLAY: play_screen(); break;
    case PAUSE: pause_screen(); break;
    case GAME_OVER: game_over(); break;
    case LOADING: loading_screen(); break;
    case ERROR_SCREEN: error_screen(); break;
    case VICTORY: victory_screen(); break;
    case LEVEL_UP: level_up(); break;
		case UNDER_CONSTRUCTION: underConstruction(); break;
		default: break;
	}
}

//mouse move
void iMouseMove(int mx, int my) {
  mouse_x = mx;
  mouse_y = my;
}

//mouse drag and move
void iMouseDrag(int mx, int my) {}

void iMouseClick(int button, int state, int mx, int my) {
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

bool echo_sound = false;
void iKeyPress(unsigned char key) {
  switch (current_screen) {
    case GAME_SCREEN:
      switch (key) {
        case '\e': //escape
          game_escape();
          break;
        case ' ':
          if (!echo_sound) {
            iPlaySound("assets/sounds/echo.wav", 0, 100);
            is_revealed = true;
            echo_sound = true;
            iResumeTimer(reveal_timer);
            srand(time(NULL));
            bool will_spawn = (rand() % 10) % (difficulty ? 4 : 2) == 0;
            if (will_spawn) spawn_monster();
          }
          break;

        default:
          break;
      }
      break;

    case NAME:
      switch (key) {
        case '\r': //enter
          if (nameLength > 0) {
            isNameGiven = true;
          }
          strcpy(playerName, temp_player_name);
          writeData(SETTINGS_FILE, TYPE_STRING, "player_name", playerName);
          break;
        case '\b': //backspace
          if (nameLength > 0) {
            nameLength--;
            temp_player_name[nameLength] = '\0';
          }
          break;
        case '\e': //escape
          temp_player_name[0] = '\0';
          current_screen = prompt_source;
          break;
        default:
          if (nameLength < 29 && key >= 32 && key <= 126) {
            temp_player_name[nameLength++] = key;
            temp_player_name[nameLength] = '\0';
          }
          break;
      }
      break;
    
    default:
    break;
  }
}

void iKeyRelease(unsigned char key) {
  switch (current_screen) {
    case GAME_SCREEN:
      switch (key) {
        case ' ': echo_sound = false; break;
      }
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

void iSpecialKeyRelease(unsigned char key) {
  switch (current_screen) {
    case GAME_SCREEN:
      is_moving = false;
      iPauseSound(running_sound);
      break;
    
    default:
      break;
  }
}

void iSpecialKeyPress(unsigned char key) {
  switch (current_screen) {
    case GAME_SCREEN:
      is_moving = true;
      iResumeSound(running_sound);
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
      }
      break;
    
    default:
      break;
  }
}

int main(int argc, char *argv[]) {
	glutInit(&argc, argv);
  reveal_timer = iSetTimer(7500, doFade);
  monster_update = iSetTimer(35, monster_ai);
  animate_timer = iSetTimer(100, animate_stuff);
  time_count = iSetTimer(1000, countTime);
  iPauseTimer(reveal_timer);
  iPauseTimer(monster_update);

  //variable data initialization from settings file
  FileData playerNameData = readData(SETTINGS_FILE, "player_name");
  isNameGiven = (bool)playerNameData.value.string[0] != '\0';
  strcpy(playerName, isNameGiven ? "Player" : playerNameData.value.string);
  background_music_setting = readData(SETTINGS_FILE, "background_music_setting").value.int_num;
  FileData difficulty_data = readData(SETTINGS_FILE, "difficulty_level");
  difficulty = difficulty_data.value.int_num;
  MONSTER_SPEED = difficulty ? 4 : 3.5;
  LOS_STEP = MONSTER_SPEED;

  //Images are to be loaded only once
  loadResources();

  waypoints[0] = player_pos;
  //debug

  iSetSpritePosition(&anim_start, 150, 200);
  background_music = iPlaySound("assets/sounds/bgm.mp3", true, 30);
  if (!background_music_setting) iPauseSound(background_music);
  running_sound = iPlaySound("assets/sounds/running.wav", true, 50);
  iPauseSound(running_sound);
	iOpenWindow(900, 600, "Echo Caves");
	return 0;
}

/*
 * Define screens here..........
 * 
 * Declare them on top first
 */

void startScreen() { //-done
  iSetColor(1, 0, 20);
  iFilledRectangle(0, 0, 900, 600);
  iShowImage(200, 475, "assets/logo.png");
	iShowImage(787, 5, "assets/texts/version.png");
  if(!isNameGiven)
    button("play_button", 595, 320, 100, 100, &current_screen, NAME);
  else
    button("play_button", 595, 320, 100, 100, &current_screen, PLAY);
  button("settings_button", 510, 224, 270, 60, &current_screen, SETTINGS);
  button("leaderboard_button", 510, 162, 270, 60, &current_screen, LEADERBOARD);
  button("story_button", 510, 100, 270, 60, &current_screen, STORY);

  button("help_button", 80, 10, 60, 60, &current_screen, INSTRUCTIONS);
  int is_exit_pressed;
  button("exit_button", 10, 10, 60, 60, &is_exit_pressed, 1);
  if (is_exit_pressed) iCloseWindow();

  iSetColor(255, 255, 255);
  iShowText(12, 80, "Name: ", CUSTOM_FONT, 18);
  iShowText(90, 80, playerName, CUSTOM_FONT, 18);
  iShowSprite(&anim_start);
}

void instructions_screen() { //-done
  iSetColor(1, 0, 20);
  iFilledRectangle(0, 0, 900, 600);
  button("back2", 15, 540, 40, 40, &current_screen, START, 0);
  iSetColor(255, 255, 255);
  iShowText(320, 550, "instructions", CUSTOM_FONT, 30);
  iShowText(50, 480, "1. Movement: use arrow keys to move around", CUSTOM_FONT, 18);
  iShowText(50, 440, "2. Use space key to reveal an area around the player. but", CUSTOM_FONT, 18);
  iShowText(50, 410, "   be careful there's a chance that the echo will spwan", CUSTOM_FONT, 18);
  iShowText(50, 380, "   a monster", CUSTOM_FONT, 18);
  iShowText(50, 340, "3. Your goal is to escape the cave", CUSTOM_FONT, 18);
  iShowText(50, 310, "   There are multiple levels in the cave and in each level", CUSTOM_FONT, 18);
  iShowText(50, 280, "   there are some stars. find and collect all the stars to", CUSTOM_FONT, 18);
  iShowText(50, 250, "   complete the level", CUSTOM_FONT, 18);
  iShowText(50, 210, "4. The monster once spawned can trace the trails you leave", CUSTOM_FONT, 18);
  iShowText(50, 180, "   behind. keep a certain distance from the monster and take", CUSTOM_FONT, 18);
  iShowText(50, 150, "   cover and eventually it will stop chasing you", CUSTOM_FONT, 18);
  iShowText(50, 110, "5. if the monster reaches up to you, the game is over. you", CUSTOM_FONT, 18);
  iShowText(50, 80, "   can either give up or restart from last save", CUSTOM_FONT, 18);
  iShowText(370, 30, "Good Luck!", CUSTOM_FONT, 18);
}

void game_screen() { 
  iSetColor(1, 0, 20);
  iFilledRectangle(0, 0, 900, 600);
  iPauseSound(background_music);
  iShowLoadedImage(background_pos.x, background_pos.y, &current_background);

  if (player_pos.x > monster_pos.x)
    animation_direction = 2;
  else
    animation_direction = 1;

  if (player_pos.y < monster_pos.y)
    switch (monster_state * animation_direction) {
      case 0:
        iSetSpritePosition(&monster_idle, monster_pos.x, monster_pos.y);
        iShowSprite(&monster_idle);
        break;
      case 1:
        iSetSpritePosition(&monster_walk_left, monster_pos.x, monster_pos.y);
        iShowSprite(&monster_walk_left);
        break;
      case 2:
        iSetSpritePosition(&monster_walk_right, monster_pos.x, monster_pos.y);
        iShowSprite(&monster_walk_right);
        break;
    }

  switch (character_direction * 2 + is_moving) {
    case 0:
      iSetSpritePosition(&idle_left, player_pos.x, player_pos.y);
      iShowSprite(&idle_left);
      break;
    case 1:
      iSetSpritePosition(&walk_left, player_pos.x, player_pos.y);
      iShowSprite(&walk_left);
      break;
    case 2:
      iSetSpritePosition(&idle_right, player_pos.x, player_pos.y);
      iShowSprite(&idle_right);
      break;
    case 3:
      iSetSpritePosition(&walk_right, player_pos.x, player_pos.y);
      iShowSprite(&walk_right);
      break;
    case 4:
      iSetSpritePosition(&idle_front, player_pos.x, player_pos.y);
      iShowSprite(&idle_front);
      break;
    case 5:
      iSetSpritePosition(&walk_front, player_pos.x, player_pos.y);
      iShowSprite(&walk_front);
      break;
    case 6:
      iSetSpritePosition(&idle_back, player_pos.x, player_pos.y);
      iShowSprite(&idle_back);
      break;
    case 7:
      iSetSpritePosition(&walk_back, player_pos.x, player_pos.y);
      iShowSprite(&walk_back);
      break;
    default:
      break;
  }

  if (!(player_pos.y < monster_pos.y))
    switch (monster_state * animation_direction) {
      case 0:
        iSetSpritePosition(&monster_idle, monster_pos.x, monster_pos.y);
        iShowSprite(&monster_idle);
        break;
      case 1:
        iSetSpritePosition(&monster_walk_left, monster_pos.x, monster_pos.y);
        iShowSprite(&monster_walk_left);
        break;
      case 2:
        iSetSpritePosition(&monster_walk_right, monster_pos.x, monster_pos.y);
        iShowSprite(&monster_walk_right);
        break;
    }
  
  //debug
  // iShowSprite(&cave_collision_box);
  // iShowSprite(&player_hitbox);
  // iSetSpritePosition(&monster_temp, monster_pos.x, monster_pos.y);
  // iShowSprite(&monster_temp);
  // iShowSprite(&monster_hitbox);
  // check_los(monster_pos, player_pos);
  // for (int i = 0; i < MAX_WAYPOINTS; i++)
  //   iShowLoadedImage(waypoints[i].x, waypoints[i].y, &waypoints_debug);
  
  if (!is_revealed) iFilledRectangle(0, 0, 900, 600);
  if (is_revealed) iShowLoadedImage(player_pos.x - 785, player_pos.y - 470, &reveal);
  iShowLoadedImage(player_pos.x + 16, player_pos.y + 65, &player_pointer);
  iShowLoadedImage(monster_pos.x + 12, monster_pos.y + 65, &monster_pointer);
  showTime();
  iShowLoadedImage(20, 20, &star_image);
  char text[20];
  sprintf(text, "%d/%d", stars_collected, MAX_STARS);
  iShowText(70, 30, text, CUSTOM_FONT, 22);
  sprintf(text, "level: %d", current_level);
  iShowText(750, 30, text, CUSTOM_FONT, 22);
  int pause_button = 0;
  button("pause", 5, 535, 60, 60, &pause_button, 1);
  if (pause_button) game_escape();
}

void storyScreen() { //-done
	iShowImage(0, 0, "assets/backgrounds/story_bg.png");
	iShowImage(98, 200, "assets/texts/story.png");
  button("back", 390, 70, 102, 25, &current_screen, START, 0);
}

void settings_screen() { //-done
  iSetColor(75, 54, 33);
  iShowImage(0,0,"assets/backgrounds/story_bg.png");
  iShowText(375, 500, "settings", CUSTOM_FONT, 24);
  char text[50];

  sprintf(text, "name: %.23s%s", playerName, strlen(playerName) > 20 ? "...": "");
  iShowText(175, 440, text, CUSTOM_FONT, 18);
  iShowText(630, 440, "- change", CUSTOM_FONT, 18);
  int button_action = 0;
  button("empty", 625, 435, 120, 25, &button_action, 1, 0);
  if (button_action) current_screen = NAME, prompt_source = SETTINGS;

  int temp_check = background_music_setting;
  sprintf(text, "background music: %s", background_music_setting ? "ON" : "OFF");
  iShowText(175, 400, text, CUSTOM_FONT, 18);
  toggle("background_music_setting", 675, 390, &background_music_setting);
  if (temp_check != difficulty) if (background_music_setting) iResumeSound(background_music); else iPauseSound(background_music);

  sprintf(text, "difficulty: %s", difficulty ? "hard" : "normal");
  iShowText(175, 360, text, CUSTOM_FONT, 18);
  iShowText(630, 360, "- change", CUSTOM_FONT, 18);
  temp_check = difficulty;
  button("empty", 625, 355, 120, 25, &difficulty, !difficulty, 0);
  if (difficulty != temp_check) {
    MONSTER_SPEED = difficulty ? 4 : 3.5;
    LOS_STEP = MONSTER_SPEED;
    writeData(SETTINGS_FILE, TYPE_INTEGER, "difficulty_level", &difficulty);
  }

  button("back", 390, 70, 102, 25, &current_screen, START, 0);
}

void prompt_screen() { //-done
  iSetColor(75, 54, 33);
  iShowImage(0,0,"assets/backgrounds/story_bg.png");
  iShowText(100, 375, "Enter your name:", CUSTOM_FONT, 18);
  iShowText(330, 375, temp_player_name, CUSTOM_FONT, 18);
  iShowText(100, 300, "Maximum characters: 30", CUSTOM_FONT, 18);
  iShowText(100, 275, "Press 'ENTER' to continue or 'Esc' to go back to the", CUSTOM_FONT, 18);
  iShowText(100, 250, "main screen", CUSTOM_FONT, 18);
}

void game_over() { //-done
  static bool sound_played = false;
  iSetColor(1, 0, 20);
  iFilledRectangle(0, 0, 900, 600);
  iSetColor(255, 255, 255);
  iShowText(305, 440, "Game over", CUSTOM_FONT, 42);
  iSetColor(230, 60, 0);
  iShowText(370, 400, "you loose", CUSTOM_FONT, 24);
  iSetColor(255, 255, 255);
  iShowText(130, 50, "N.B. if you restart, you will start from last save", CUSTOM_FONT, 18);
  if (!sound_played) iPlaySound("assets/sounds/game_over.wav", false, 80), sound_played = true;
  int button_action = -1;
  button("restart", 320, 260, 270, 60, &button_action, 0);
  button("main_menu", 320, 198, 270, 60, &button_action, 1);
  switch (button_action) {
    case 0:
      sound_played = false;
      read_saved_game();
      if (monster_state - 2) iResumeTimer(monster_update);
      level_init = 2; 
      level_up();
      loading_destination = GAME_SCREEN;
      current_screen = LOADING;
      break;
    case 1: sound_played = false; current_screen = START; break;
  }
}

void victory_screen() { //-done
  static bool sound_played = false;
  iSetColor(1, 0, 20);
  iFilledRectangle(0, 0, 900, 600);
  iSetColor(255, 255, 255);
  iShowText(200, 450, "Congratulations!", CUSTOM_FONT, 42);
  iSetColor(0, 200, 40);
  iShowText(390, 410, "you win!", CUSTOM_FONT, 24);
  char timeText[20];
  sprintf(timeText, "Time: %02d:%02d:%02d", hours, mins, sec);
  iSetColor(255, 255, 255);
  iShowText(350, 300, timeText, CUSTOM_FONT, 22);
  if (!sound_played) iPlaySound("assets/sounds/victory.mp3", false, 80), sound_played = true;
  int button_action = -1;
  button("main_menu", 320, 150, 270, 60, &button_action, 0);
  switch (button_action) {
    case 0: sound_played = false; loading_destination = START; current_screen = LOADING; reset_game(); updateLeaderboard(); break;
  }
}

void level_up() {
  static bool sound_played = false;
  if (!level_init) {
    iSetColor(1, 0, 20);
    iFilledRectangle(0, 0, 900, 600);
    iSetColor(255, 255, 255);
    iShowText(210, 450, "Level Complete!", CUSTOM_FONT, 42);
    iShowText(240, 390, "continue to next level", CUSTOM_FONT, 24);
    iShowLoadedImage(380, 320, &star_image);
    iShowLoadedImage(430, 320, &star_image);
    iShowLoadedImage(480, 320, &star_image);
    if (!sound_played) iPlaySound("assets/sounds/level_up.mp3", false, 80), sound_played = true;
    int button_action = -1;
    button("continue", 320, 200, 270, 60, &button_action, 0);
    button("main_menu", 320, 138, 270, 60, &button_action, 1);
    switch (button_action) {
      case 0: sound_played = false; current_screen = PLAY; stars_collected = 0; break;
      case 1: save_game(); current_screen = START; break;
    }
    monster_state = 0;
    for (int i = 0; i < MAX_WAYPOINTS; i++) {
      waypoints[i] = {0, 0};
    }
  }

  switch (current_level) { //todo
    case 1:
      if (level_init != 2) { 
        player_pos = {100, 344};
        background_pos = {-55, -600};
        stars_collected = 0;
      }
      current_background = cave_background;
      if (!level_init) current_level = 2;
      break;
    case 2:
      break;
    case 3:
      break;
  }
}

void error_screen() { //-done
  iSetColor(1, 0, 20);
  iFilledRectangle(0, 0, 900, 600);
  iSetColor(255, 255, 255);
  iShowText(160, 430, "An Error has occured!", CUSTOM_FONT, 36);
  button("main_menu", 289, 220, 270, 60, &current_screen, START);
  button("exit_button", 561, 220, 60, 60, &current_screen, START);
}

void pause_screen() { //-done
  iSetColor(1, 0, 20);
  iFilledRectangle(0, 0, 900, 600);
  iSetColor(255, 255, 255);
  iShowText(290, 425, "game paused", CUSTOM_FONT, 36);
  int button_action = -1;
  button("resume", 320, 270, 270, 60, &button_action, 0);
  button("savenquit", 320, 208, 270, 60, &button_action, 1);
  button("main_menu", 320, 146, 270, 60, &current_screen, START);
  switch (button_action) {
    case 0: if (monster_state - 2) iResumeTimer(monster_update); current_screen = GAME_SCREEN; break;
    case 1: save_game(); loading_destination = START; current_screen = LOADING; break;
  }
}

void showLeaderboard() { //-done
  //leaderboard ekn show korte hobe
  iShowImage(0, 0, "assets/backgrounds/story_bg.png");
  iSetColor(75, 54, 33);
  iShowText(350, 500, "LEADERBOARD", CUSTOM_FONT, 24);
  int count = 0;
  static FileData leaderboard[MAX_LEADERBOARD_DISPLAY];

  static bool hasOpened = true;
  if (hasOpened) {
    FILE *pFile = fopen(LEADERBOARD_FILE, "rb");
    if (pFile == NULL) return;
    FileData buffer;
    while (fread(&buffer, ENTRY_SIZE, 1, pFile) == 1) {
      leaderboard[count] = buffer;
      count++;
    }
    fclose(pFile);
    hasOpened = false;
  }

  int y = 430;
  char number[3];
  for (int i = 0; i < MAX_LEADERBOARD_DISPLAY; i++) {
    sprintf(number, "%d.", i + 1);
    iShowText(160, y, number, CUSTOM_FONT, 18);
    iShowText(190, y, leaderboard[i].id[0] == '\0' ? "player" : leaderboard[i].id, CUSTOM_FONT, 18);
    iShowText(640, y, leaderboard[i].value.leaderboard.time[0] == '\0' ? "00:00:00" : leaderboard[i].value.leaderboard.time, CUSTOM_FONT, 18);
    y -= 30;
  }

  int button_action = -1;
  iShowText(405, 120, "reset", CUSTOM_FONT, 20);
  button("empty", 400, 115, 100, 25, &button_action, 1, 0);
  button("back", 390, 70, 102, 25, &button_action, 0, 0);
  switch (button_action) {
    case 0: current_screen = START; hasOpened = true; break;
    case 1: current_screen = START; clear_file_data(LEADERBOARD_FILE); hasOpened = true; break;
  }
}

void play_screen() { //-done
  iSetColor(1, 0, 20);
  iFilledRectangle(0, 0, 900, 600);
  iSetColor(255, 255, 255);
  iShowText(300, 475, "Start a new game", CUSTOM_FONT, 24);
  iShowText(263, 435, "or continue existing", CUSTOM_FONT, 24);
  int button_action = -1;
  button("new_game", 320, 270, 270, 60, &button_action, 0);
  button("continue", 320, 208, 270, 60, &button_action, 1);
  switch (button_action) {
    case 0:
      reset_game();
      level_init = 1; 
      level_up();
      loading_destination = GAME_SCREEN;
      current_screen = LOADING;
      break;
    case 1:
      read_saved_game();
      level_up();
      if (monster_state - 2) iResumeTimer(monster_update);
      loading_destination = GAME_SCREEN;
      current_screen = LOADING;
      break;
  }
  button("back2", 15, 540, 40, 40, &current_screen, START, 0);
}

void loading_screen() { //-done
  static int frame_count = 0;
  if (!frame_count) srand(time(NULL));
  frame_count++;
  iSetColor(1, 0, 20);
  iFilledRectangle(0, 0, 900, 600);
  iShowSprite(&loading);
  iSetColor(255, 255, 255);
  iShowText(380, 250, "Loading...", CUSTOM_FONT, 24);
  if (frame_count > (250 + rand() % (500 - 250 + 1))) current_screen = loading_destination, frame_count = 0;
}

void underConstruction() { //-done
  iSetColor(255, 255, 255);
	iShowText(10, 10, "Under Construction...   Come back later (Press q to go back)", CUSTOM_FONT, 18);
}

/*
 * components
 */

void button(const char texture_name[], int pos_x, int pos_y, int width, int height, int *var_name, int var_value, int has_state, int make_sound) { //max length of texture path is 100
  
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
  static bool button_sound = false;
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
    case 0: strcat(texture_path, ".png"); break;
    case 1: strcat(texture_path, has_state ? "_hover.png" : ".png"); break;
    case 2: strcat(texture_path, has_state ? "_pressed.png" : ".png"); break;
  }
  iShowImage(pos_x, pos_y, texture_path);
}

void toggle(const char toggle_name[], int pos_x, int pos_y, bool *var_name, int make_sound) { //toggle_name will be used to store and retrieve data from file, max length of texture path is 100
  
  /* Notes before using
   * Kind of same as button
   * Use boolean variable for var_name
   */

  static bool hasRun = false;
  if (!hasRun) {
    FileData data = readData(SETTINGS_FILE, toggle_name);
    *var_name = readData(SETTINGS_FILE, toggle_name).value.int_num;
    hasRun = true;
  }

  static int state = readData(SETTINGS_FILE, toggle_name).value.int_num * 2; //0, 1 initialization
  char texture_path[100] = "assets/buttons/toggle_";
  static bool button_sound = false;
  static bool was_clicked = false;

  if ((mouse_x > pos_x && mouse_x < (pos_x + 58)) && (mouse_y > pos_y && mouse_y < (pos_y + 32))) {
    switch (state) {
      case 0: state = 1; break;
      case 2: state = 3; break;
    }

    if (is_left_button) {
      if (is_state_down) {
        if (!was_clicked) {
          switch (state) {
            case 1: state = 3; break;
            case 3: state = 1; break;
          }
          int save_state = (state - 1) / 2;
          writeData(SETTINGS_FILE, TYPE_INTEGER, toggle_name, &save_state);
          was_clicked = true;
        }
        if (!button_sound && make_sound) {
          iPlaySound("assets/sounds/button_click.wav", false, 80);
          button_sound = true;
        }
        button_click = true;
      } else if (is_state_up) {
        switch (state) {
          case 1: *var_name = false; break;
          case 3: *var_name = true; break;
        }
        was_clicked = false;
        button_sound =  false;
        button_click = false;

        //reset mouse states
        is_state_up = false;
        is_state_down = false;
        is_left_button = false;
      }
    }
  } else switch (state) {
    case 1: state = 0; break;
    case 3: state = 2; break;
  }

  switch (state) {
    case 0: strcat(texture_path, "unchecked.png"); break;
    case 1: strcat(texture_path, "unchecked_hover.png"); break;
    case 2: strcat(texture_path, "checked.png"); break;
    case 3: strcat(texture_path, "checked_hover.png"); break;
  }

  iShowImage(pos_x, pos_y, texture_path);
}

/*
 * Read and Write Functions
 **************************
 * enum - destination_file: SETTINGS_FILE, ...
 * enum - type_of_data: TYPE_STRING, TYPE_INT, ...
 * string - id: any name
 * pointer - value1, value2, ...
 */

void writeData(const char destination_file[], FileDataType type_of_data, const char id[], void *value1, void *value2) {
  FILE *pFile = NULL;
  pFile = fopen(destination_file, "r+b");
  if (!pFile) return;

  FileData buffer;
  long offset = 0;
  FileData data = {0};
  bool is_match_found = false;
  int entry_count = 0;

  while (fread(&buffer, ENTRY_SIZE, 1, pFile) == 1) {
    if (!strncmp(buffer.id, id, ENTRY_ID_SIZE)) {
      is_match_found = true;
      fseek(pFile, offset, SEEK_SET); // Rewind to start of this record
      break;
    }
    offset += ENTRY_SIZE;
  }

  if (!is_match_found)
    fseek(pFile, 0, SEEK_END);

  switch (type_of_data) {
    case TYPE_INTEGER:
      strncpy(data.id, id, ENTRY_ID_SIZE - 1);
      data.id[ENTRY_ID_SIZE - 1] = '\0';
      data.value.int_num = *(int *)value1;
      fwrite(&data, ENTRY_SIZE, 1, pFile);
      break;

    case TYPE_STRING:
      strncpy(data.id, id, ENTRY_ID_SIZE - 1);
      data.id[ENTRY_ID_SIZE - 1] = '\0';
      strncpy(data.value.string, (char *)value1, sizeof(data.value.string) - 1);
      data.value.string[sizeof(data.value.string) - 1] = '\0';
      fwrite(&data, ENTRY_SIZE, 1, pFile);
      break;

    case TYPE_FLOAT:
      strncpy(data.id, id, ENTRY_ID_SIZE - 1);
      data.id[ENTRY_ID_SIZE - 1] = '\0';
      data.value.float_num = *(float *)value1;
      fwrite(&data, ENTRY_SIZE, 1, pFile);
      break;

    case TYPE_VEC2:
      strncpy(data.id, id, ENTRY_ID_SIZE - 1);
      data.id[ENTRY_ID_SIZE - 1] = '\0';
      data.value.vec2.x = *((float *)value1);
      data.value.vec2.y = *((float *)value2);
      fwrite(&data, ENTRY_SIZE, 1, pFile);
      break;

    case TYPE_LEADERBOARD:
      fseek(pFile, 0, SEEK_END);
      strncpy(data.id, id, ENTRY_ID_SIZE - 1);
      data.id[ENTRY_ID_SIZE - 1] = '\0';
      data.value.leaderboard.seconds = *((int *)value1);
      strncpy(data.value.leaderboard.time, (char *)value2, sizeof(data.value.leaderboard.time) - 1);
      data.value.leaderboard.time[sizeof(data.value.leaderboard.time) - 1] = '\0';
      fwrite(&data, ENTRY_SIZE, 1, pFile);
      break;

    case TYPE_WAYPOINTS: 
      strncpy(data.id, id, ENTRY_ID_SIZE - 1);
      data.id[ENTRY_ID_SIZE - 1] = '\0';
      for (int i = 0; i < MAX_WAYPOINTS - 2; i++) {
        data.value.waypoint[i].x = waypoints[i].x;
        data.value.waypoint[i].y = waypoints[i].y;
      }
      fwrite(&data, ENTRY_SIZE, 1, pFile);
      break;

    default:
      break;
  }

  fclose(pFile);
}

FileData readData(const char source_file[], const char id[], bool is_array) {
  FILE *pFile = fopen(source_file, "rb");
  FileData empty = {0};
  if (!pFile) return empty;

  FileData buffer;
  bool is_match_found = false;

  while (fread(&buffer, ENTRY_SIZE, 1, pFile) == 1) {
    if (!strncmp(buffer.id, id, ENTRY_ID_SIZE)) {
      is_match_found = true;
      break;
    }
  }

  if (is_match_found) {
    if (is_array) {
      for (int i = 0; i < MAX_WAYPOINTS - 2; i++) {
        waypoints[i].x = buffer.value.waypoint[i].x;
        waypoints[i].y = buffer.value.waypoint[i].y;
      }
      return buffer;
    } else {
      fclose(pFile);
      return buffer;
    }
  }

  fclose(pFile);
  return empty;
}

/*
 * Other necessary functions
 */

void doFade() { //-done
  if (is_revealed) is_revealed = 0;
  if (!is_revealed) iPauseTimer(reveal_timer);
}

void loadResources() { //-done
  iLoadImage(&reveal, "assets/game_screen/reveal.png");
  iLoadImage(&cave_background, "assets/game_screen/cave_background.png");
  iLoadImage(&timer_background, "assets/backgrounds/timer_bg.png");
  iLoadImage(&waypoints_debug, "assets/game_screen/waypoints_debug.png");
  iLoadImage(&player_pointer, "assets/game_screen/player_pointer.png");
  iLoadImage(&monster_pointer, "assets/game_screen/monster_pointer.png");
  iLoadImage(&star_image, "assets/game_screen/star.png");
  iLoadFramesFromSheet(idle_left_frames, "assets/game_screen/character_spritesheets/idle_left.png", 1, 8);
  iLoadFramesFromSheet(idle_right_frames, "assets/game_screen/character_spritesheets/idle_right.png", 1, 8);
  iLoadFramesFromSheet(idle_front_frames, "assets/game_screen/character_spritesheets/idle_front.png", 1, 8);
  iLoadFramesFromSheet(idle_back_frames, "assets/game_screen/character_spritesheets/idle_back.png", 1, 8);
  iLoadFramesFromSheet(walk_left_frames, "assets/game_screen/character_spritesheets/walk_left.png", 1, 8);
  iLoadFramesFromSheet(walk_right_frames, "assets/game_screen/character_spritesheets/walk_right.png", 1, 8);
  iLoadFramesFromSheet(walk_front_frames, "assets/game_screen/character_spritesheets/walk_front.png", 1, 8);
  iLoadFramesFromSheet(walk_back_frames, "assets/game_screen/character_spritesheets/walk_back.png", 1, 8);
  iLoadFramesFromSheet(anim_start_player, "assets/game_screen/character_spritesheets/anim_start_player.png", 1, 8);
  iLoadFramesFromSheet(loading_image, "assets/game_screen/loading_spin.png", 1, 10);
  iLoadFramesFromSheet(monster_idle_frames, "assets/game_screen/monster_sprites/monster_idle.png", 1, 6);
  iLoadFramesFromSheet(monster_walk_right_frames, "assets/game_screen/monster_sprites/monster_walk_right.png", 1, 8);
  iLoadFramesFromSheet(monster_walk_left_frames, "assets/game_screen/monster_sprites/monster_walk_left.png", 1, 8);
	iChangeSpriteFrames(&idle_left, idle_left_frames, 8);
  iChangeSpriteFrames(&idle_right, idle_right_frames, 8);
  iChangeSpriteFrames(&idle_front, idle_front_frames, 8);
  iChangeSpriteFrames(&idle_back, idle_back_frames, 8);
  iChangeSpriteFrames(&walk_left, walk_left_frames, 8);
  iChangeSpriteFrames(&walk_right, walk_right_frames, 8);
  iChangeSpriteFrames(&walk_front, walk_front_frames, 8);
  iChangeSpriteFrames(&walk_back, walk_back_frames, 8);
  iChangeSpriteFrames(&anim_start, anim_start_player, 8);
  iChangeSpriteFrames(&loading, loading_image, 10);
	iChangeSpriteFrames(&monster_idle, monster_idle_frames, 6);
  iChangeSpriteFrames(&monster_walk_right, monster_walk_right_frames, 8);
  iChangeSpriteFrames(&monster_walk_left, monster_walk_left_frames, 8);
  iSetSpritePosition(&loading, 436, 300);
  iLoadImage(&player_hitbox_image, "assets/game_screen/character_spritesheets/player_hitbox.png");
  iChangeSpriteFrames(&player_hitbox, &player_hitbox_image, 1);
  iLoadImage(&monster_hitbox_image, "assets/game_screen/monster_sprites/monster_hitbox.png");
  iChangeSpriteFrames(&monster_hitbox, &monster_hitbox_image, 1);
  //debug
  iLoadImage(&monster_temp_image, "assets/game_screen/monster_sprites/monster_temp.png");
  iChangeSpriteFrames(&monster_temp, &monster_temp_image, 1);

  iLoadImage(&cave_collision_box_image, "assets/game_screen/cave_collision_box.png");
  iChangeSpriteFrames(&cave_collision_box, &cave_collision_box_image, 1);
  iSetSpritePosition(&player_hitbox, player_pos.x, player_pos.y);
  iLoadImage(&los_checker_image, "assets/game_screen/los_checker.png");
  iChangeSpriteFrames(&los_checker, &los_checker_image, 1);
}

void animate_stuff() { //-done
  static int frame_count;
  frame_count++;
  iAnimateSprite(&anim_start);
  iAnimateSprite(&loading);
  switch (monster_state * animation_direction) {
    case 0:
      iAnimateSprite(&monster_idle);
      if (frame_count > 12) iMirrorSprite(&monster_idle, HORIZONTAL), frame_count = 0;
      break;
    case 1:
      iAnimateSprite(&monster_walk_left);
      break;
    case 2:
      iAnimateSprite(&monster_walk_right);
      break;
  }
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

void move_player(int direction) { //-done
  float offset = PLAYER_SPEED;
  int offset_direction = 0;
  float dx = PLAYER_SPEED, dy = PLAYER_SPEED;
  bool use_offset;
  float tentative_x = player_pos.x;
  float tentative_y = player_pos.y;

  switch (direction) {
    case 0: tentative_x -= PLAYER_SPEED; break;
    case 1: tentative_x += PLAYER_SPEED; break;
    case 2: tentative_y -= PLAYER_SPEED; break;
    case 3: tentative_y += PLAYER_SPEED; break;
  }

  iSetSpritePosition(&player_hitbox, tentative_x, tentative_y);
  if (!iCheckCollision(&cave_collision_box, &player_hitbox)) {
    player_pos.x = tentative_x;
    player_pos.y = tentative_y;
  }

  if (player_pos.x < 100 || player_pos.x > 800 || player_pos.y < 80 || player_pos.y > 480)
    use_offset = true;
  else use_offset = false;

  if (player_pos.x < 100) {
    player_pos.x = 100;
    background_pos.x += PLAYER_SPEED;
  } else if (player_pos.x > 800) {
    player_pos.x = 800;
    background_pos.x -= PLAYER_SPEED;
    offset = dx = -PLAYER_SPEED;
  }

  if (player_pos.y < 80) {
    player_pos.y = 80;
    background_pos.y += PLAYER_SPEED;
    offset_direction = 1;
  } else if (player_pos.y > 480) {
    player_pos.y = 480;
    background_pos.y -= PLAYER_SPEED;
    offset = dy = -PLAYER_SPEED;
    offset_direction = 1;
  }

  iSetSpritePosition(&player_hitbox, player_pos.x, player_pos.y);
  iSetSpritePosition(&cave_collision_box, background_pos.x, background_pos.y);
  if (use_offset) offset_waypoints(offset, offset_direction), offset_monster(dx, dy, offset_direction);
  update_waypoints();
}

bool spawn_monster() {
  if (monster_state != 2) return false;
  const float radius = 225;
  const int angle_step = 15; // try every 15 degrees (360 / 15 = 24 attempts)
  for (int deg = 0; deg < 360; deg += angle_step) {
    float rad = deg * (M_PI / 180.0f);
    Vec2 candidate = {
      player_pos.x + cos(rad) * radius,
      player_pos.y + sin(rad) * radius
    };

    iSetSpritePosition(&monster_hitbox, candidate.x, candidate.y);
    if (!iCheckCollision(&cave_collision_box, &monster_hitbox) && check_los(candidate, player_pos)) {
      monster_pos = candidate;
      monster_state = 1;
      iResumeTimer(monster_update);
      return true;
    }
  }
  return false;
}

void monster_ai() {
  if (monster_state == 2) return;

  if (check_los(monster_pos, player_pos)) {
    monster_target = player_pos;
    has_target = true;
    monster_idle_timer = 0;
  } else {
    has_target = false;
    for (int i = 0; i < MAX_WAYPOINTS; ++i) {
      if (check_los(monster_pos, waypoints[i]) && check_los(waypoints[i], player_pos)) {
        monster_target = waypoints[i];
        has_target = true;
        break;
      }
    }

    if (!has_target) {
      monster_idle_timer++;
      monster_state = 0;
      if (monster_idle_timer > 170) {
        monster_state = 2;
        iPauseTimer(monster_update);
        monster_pos.x = -100;
        monster_pos.y = -100;
        iSetSpritePosition(&monster_hitbox, monster_pos.x, monster_pos.y);
        return;
      }
      return;
    }
  }

  float dx = monster_target.x - monster_pos.x;
  float dy = monster_target.y - monster_pos.y;
  float dist = sqrt(dx * dx + dy * dy);

  if (dist > MONSTER_SPEED) {
    float dir_x = dx / dist;
    float dir_y = dy / dist;

    float tentative_x = monster_pos.x + dir_x * MONSTER_SPEED;
    float tentative_y = monster_pos.y + dir_y * MONSTER_SPEED;

    iSetSpritePosition(&monster_hitbox, tentative_x, tentative_y);
    if (!iCheckCollision(&cave_collision_box, &monster_hitbox)) {
      monster_pos.x = tentative_x;
      monster_pos.y = tentative_y;
    }
  } else {
    monster_pos = monster_target;
    game_escape(GAME_OVER);
  }

  iSetSpritePosition(&monster_hitbox, monster_pos.x, monster_pos.y);
}

void update_waypoints() { //-done
  float dx = player_pos.x - waypoints[0].x;
  float dy = player_pos.y - waypoints[0].y;
  float dist = sqrtf(dx * dx + dy * dy);

  if (dist >= WAYPOINT_SPACING) {
    for (int i = MAX_WAYPOINTS - 1; i > 0; --i) {
      waypoints[i] = waypoints[i - 1];
    }
    waypoints[0] = player_pos;
  }
}

void offset_waypoints(int offset, int direction) { //-done
  for (int i = 0; i < MAX_WAYPOINTS; i++)
    switch (direction) {
      case 0: waypoints[i].x += offset; break;
      case 1: waypoints[i].y += offset; break;
    }
}

void offset_monster(float dx, float dy, int direction) { //-done
  if (monster_state != 2) {
    switch (direction) {
      case 0: monster_pos.x += dx; break;
      case 1: monster_pos.y += dy; break;
    }
    iSetSpritePosition(&monster_hitbox, monster_pos.x, monster_pos.y);
  }
}

bool check_los(Vec2 from, Vec2 to) { //-done
  float dx = to.x - from.x;
  float dy = to.y - from.y;
  float dist = sqrt(dx * dx + dy * dy);

  float dir_x = dx / dist;
  float dir_y = dy / dist;

  float current_x = from.x;
  float current_y = from.y;

  for (float traveled = 0; traveled < dist; traveled += LOS_STEP) {
    iSetSpritePosition(&los_checker, current_x, current_y);
    //debug
    // iShowSprite(&los_checker);
    if (iCheckCollision(&cave_collision_box, &los_checker)) {
      return false; // blocked
    }
    current_x += dir_x * LOS_STEP;
    current_y += dir_y * LOS_STEP;
  }

  return true;
}

void countTime() { //-done
  if (current_screen == GAME_SCREEN)
    secPassed++;
}

void showTime() { //-done
  if (current_screen == GAME_SCREEN) {
    mins = secPassed / 60;
    sec = secPassed % 60;
    hours = mins/60;
    char timeText[20];
    sprintf(timeText, "%02d:%02d:%02d", hours, mins, sec);
    iSetColor(255, 255, 255);
    iShowLoadedImage(775, 565, &timer_background);
    iShowText(786, 574, timeText, CUSTOM_FONT, 18);
  }
}

void save_game() {
  writeData(GAME_DATA, TYPE_INTEGER, "current_level", &current_level);
  writeData(GAME_DATA, TYPE_INTEGER, "monster_state", &monster_state);
  writeData(GAME_DATA, TYPE_INTEGER, "seconds_passed", &secPassed);
  writeData(GAME_DATA, TYPE_INTEGER, "stars_collected", &stars_collected);
  writeData(GAME_DATA, TYPE_VEC2, "background_pos", &background_pos.x, &background_pos.y);
  writeData(GAME_DATA, TYPE_VEC2, "player_pos", &player_pos.x, &player_pos.y);
  writeData(GAME_DATA, TYPE_VEC2, "monster_pos", &monster_pos.x, &monster_pos.y);
  writeData(GAME_DATA, TYPE_WAYPOINTS, "waypoints", NULL);
}

void read_saved_game() {
  FileData data = readData(GAME_DATA, "current_level");
  if (data.id[0] != '\0') {
    current_level = data.value.int_num;
    data = readData(GAME_DATA, "monster_state");
    monster_state = data.value.int_num;
    data = readData(GAME_DATA, "seconds_passed");
    secPassed = data.value.int_num;
    data = readData(GAME_DATA, "stars_collected");
    stars_collected = data.value.int_num;
    data = readData(GAME_DATA, "background_pos");
    background_pos.x = data.value.vec2.x;
    background_pos.y = data.value.vec2.y;
    data = readData(GAME_DATA, "player_pos");
    player_pos.x = data.value.vec2.x;
    player_pos.y = data.value.vec2.y;
    data = readData(GAME_DATA, "monster_pos");
    monster_pos.x = data.value.vec2.x;
    monster_pos.y = data.value.vec2.y;
    readData(GAME_DATA, "waypoints", true);
    level_init = 2;
  } else {
    level_init = 1;
    reset_game();
  }
}

void reset_game() {
  current_level = 1;
  monster_state = 2;
  secPassed = 0;
  hours = 0;
  mins = 0;
  sec = 0;
  stars_collected = 0;
  monster_pos = {-100, -100};
  for (int i = 0; i < MAX_WAYPOINTS; i++) {
    waypoints[i] = {0, 0};
  }
}

void clear_file_data(const char file[]) { //-done
  FILE *pFile = fopen(file, "w");
  if (pFile != NULL) {
    fclose(pFile);
  } else current_screen = ERROR_SCREEN; return;
}

void game_escape(int escape) {
  iSpecialKeyRelease(GLUT_KEY_END);
  iPauseTimer(monster_update);
  if (background_music_setting) iResumeSound(background_music);
  current_screen = escape;
}

void updateLeaderboard() { //-done
  FileData leaderboard[MAX_LEADERBOARD_DISPLAY + 1];

  //added new data
  char time_str[64];
  sprintf(time_str, "%02d:%02d:%02d", hours, mins, sec);
  writeData(LEADERBOARD_FILE, TYPE_LEADERBOARD, playerName, &secPassed, time_str);

  //read and add to array
  FILE *pFile = fopen(LEADERBOARD_FILE, "rb");
  if (pFile == NULL) current_screen = ERROR_SCREEN; return;
  FileData buffer;
  int count = 0;
  while (fread(&buffer, ENTRY_SIZE, 1, pFile) == 1) {
    leaderboard[count] = buffer;
    count++;
  }
  fclose(pFile);

  //sorting
  for (int i = 0; i < count - 1; i++)
    for (int j = i + 1; j < count; j++)
      if (leaderboard[j].value.leaderboard.seconds < leaderboard[i].value.leaderboard.seconds) {
        FileData temp = leaderboard[i];
        leaderboard[i] = leaderboard[j];
        leaderboard[j] = temp;
      }

  //overriding current data in file
  pFile = fopen(LEADERBOARD_FILE, "wb");
  if (pFile == NULL) current_screen = ERROR_SCREEN; return;
  count = count > MAX_LEADERBOARD_DISPLAY ? MAX_LEADERBOARD_DISPLAY : count;
  for (int i = 0; i < count; i++) {
    FileData buffer = leaderboard[i];
    fwrite(&buffer, ENTRY_SIZE, 1, pFile);
  }
  fclose(pFile);
}