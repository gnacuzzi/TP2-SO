#ifndef ELIMINATOR_H
#define ELIMINATOR_H

#include "color.h"

#define ESC '\x1B'
#define ENTER '\n'
#define SPACEBAR ' '

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
#define SQUARE_SIZE 8
#define STARTING_OFFSET_1 0.25
#define STARTING_OFFSET_2 0.75

#define NAME_LENG 10

#define MSG_START                                                                                                      \
	"ELIMINATOR (ARQUI'S VERSION)\n PLAYERS: %d \n SPEED: %d \n\n [SPACE] to begin game \n [ENTER] to change \n "      \
	"[ESCAPE] to exit \n"
#define MSG_SPEED "SPEED (1-3): "
#define MSG_PLAYERS "PLAYERS (1-2): "

#define MSG_P1 "The first player moves with the arrows\n"
#define MSG_P2 "The second player moves with keys: WASD\n"
#define MSG_SINGLE "Play using the arrows\n"

#define POINTS "%s: %d - %s: %d \n"

void start_eliminator();

void starting_screen();

void settings();

void play();

void play1();

void play2();

void set_enviroment();

int checkWin(uint16_t x1, uint16_t y1, char *name1, uint16_t x2, uint16_t y2, char *name2);

int want_continue();

int analyseSituation();

typedef struct {
	uint16_t x, y;
	uint16_t inc_x, inc_y;
	uint8_t points;
	Color color;
	char *name;
} Player;

#endif