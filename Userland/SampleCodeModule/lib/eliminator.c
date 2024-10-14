// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <stdint.h>
#include <libc.h>
#include <eliminator.h>
#include <syscall.h>
#include <rand.h>

static int CANT_PLAYERS = 1;
static int SPEED = 1;

static char board[SCREEN_WIDTH][SCREEN_HEIGHT]; // tamaño de la pantalla

#define OUT_OF_BOUNDS(x, y) ((x) >= SCREEN_WIDTH || (x) <= 0 || (y) >= SCREEN_HEIGHT || (y) <= 0)

Player player1, player2;

void start_eliminator() {
	clearscreen();
	setlettersize(2);
	printf(MSG_START, CANT_PLAYERS, SPEED);
	char input;
	while ((input = readchar()) != SPACEBAR && input != ENTER) {
		if (input == ESC) {
			CANT_PLAYERS = 1;
			SPEED = 1;
			return;
		}
	}
	if (input == SPACEBAR) {
		if (CANT_PLAYERS == 1) {
			player1 = (Player) {SCREEN_WIDTH / 2, SCREEN_HEIGHT * STARTING_OFFSET_1, 0, 1, 0, PINK, "You"};
			player2 =
				(Player) {SCREEN_WIDTH / 2, SCREEN_HEIGHT * STARTING_OFFSET_2, 0, -1, 0, LIGHT_GREEN, "Eliminator"};
		}
		starting_screen();
	}
	else {
		settings();
	}
}

void settings() {
	clearscreen();
	printf(MSG_SPEED);
	while (1) {
		char speed[5];
		scanf(speed);
		if (strcmp(speed, "1") == 0 || strcmp(speed, "2") == 0 || strcmp(speed, "3") == 0) {
			SPEED = ctoi(speed[0]);
			break;
		}
		else {
			printf("Enter a number between 1 and 3 \n");
		}
	}
	putchar(ENTER);

	printf(MSG_PLAYERS);
	while (1) {
		char cant[5];
		scanf(cant);
		if (strcmp(cant, "1") == 0 || strcmp(cant, "2") == 0) {
			CANT_PLAYERS = ctoi(cant[0]);
			break;
		}
		else {
			printf("Enter 1 or 2 \n");
		}
	}
	putchar(ENTER);

	if (CANT_PLAYERS == 2) {
		char name1[NAME_LENG];
		printf("1st player's name: ");
		scanf(name1);
		player1 = (Player) {SCREEN_WIDTH / 2, SCREEN_HEIGHT * STARTING_OFFSET_1, 0, 1, 0, PINK, name1};
		char name2[NAME_LENG];
		printf("2nd player's name: ");
		scanf(name2);
		player2 = (Player) {SCREEN_WIDTH / 2, SCREEN_HEIGHT * STARTING_OFFSET_2, 0, -1, 0, LIGHT_GREEN, name2};
	}
	start_eliminator();
}

void starting_screen() {
	clearscreen();
	if (CANT_PLAYERS == 1) {
		printf(MSG_SINGLE);
	}
	else {
		printf(MSG_P1);
		printf(MSG_P2);
	}
	printf("Click SPACE to play or ENTER to go back to the menu\n");
	char c;
	while ((c = readchar()) != ESC) {
		if (c == SPACEBAR) {
			play();
			start_eliminator();
		}
		if (c == ENTER) {
			start_eliminator();
		}
	}
}

void play() {
	while (1) {
		if (CANT_PLAYERS == 1) {
			play1();
		}
		else {
			play2();
		}
		if (want_continue() == 0) {
			break;
		}
	}
}

int want_continue() {
	printf(POINTS, player1.name, player1.points, player2.name, player2.points);
	printf("Press [SPACE] to cotinue or [ENTER] to change options or to return");
	while (1) {
		char c = readchar();
		if (c == SPACEBAR) {
			return 1;
		}
		else if (c == ENTER) {
			return 0;
		}
	}
}

void play1() {
	set_enviroment();
	char c;
	while (1) {
		wait_delta(1 / (SPEED * 0.3));
		c = readchar();
		switch (c) {
			case 2: // left
				if (player1.inc_x == 0) {
					player1.inc_x = -1;
					player1.inc_y = 0;
				}
				break;
			case 3: // right
				if (player1.inc_x == 0) {
					player1.inc_x = 1;
					player1.inc_y = 0;
				}
				break;
			case 4: // up
				if (player1.inc_y == 0) {
					player1.inc_x = 0;
					player1.inc_y = -1;
				}
				break;
			case 5: // down
				if (player1.inc_y == 0) {
					player1.inc_x = 0;
					player1.inc_y = 1;
				}
				break;
		}
		srand(get_ticks());
		char key = rand_char();
		switch (key) {
			case 'W':
				if (player2.inc_y == 0) {
					player2.inc_y = -1;
					player2.inc_x = 0;
				}
				break;
			case 'A':
				if (player2.inc_x == 0) {
					player2.inc_y = 0;
					player2.inc_x = -1;
				}
				break;
			case 'S':
				if (player2.inc_y == 0) {
					player2.inc_y = 1;
					player2.inc_x = 0;
				}
				break;
			case 'D':
				if (player2.inc_x == 0) {
					player2.inc_y = 0;
					player2.inc_x = 1;
				}
				break;
		}

		if (analyseSituation()) {
			return;
		}
	}
	clear_screen();
}

void play2() {
	set_enviroment();
	char c;
	while (1) {
		wait_delta(1 / (SPEED * 0.3));
		c = readchar();
		switch (c) {
			case 'W':
				if (player2.inc_y == 0) {
					player2.inc_y = -1;
					player2.inc_x = 0;
				}
				break;
			case 'A':
				if (player2.inc_x == 0) {
					player2.inc_y = 0;
					player2.inc_x = -1;
				}
				break;
			case 'S':
				if (player2.inc_y == 0) {
					player2.inc_y = 1;
					player2.inc_x = 0;
				}
				break;
			case 'D':
				if (player2.inc_x == 0) {
					player2.inc_y = 0;
					player2.inc_x = 1;
				}
				break;
			case 2: // left
				if (player1.inc_x == 0) {
					player1.inc_x = -1;
					player1.inc_y = 0;
				}
				break;
			case 3: // right
				if (player1.inc_x == 0) {
					player1.inc_x = 1;
					player1.inc_y = 0;
				}
				break;
			case 4: // up
				if (player1.inc_y == 0) {
					player1.inc_x = 0;
					player1.inc_y = -1;
				}
				break;
			case 5: // down
				if (player1.inc_y == 0) {
					player1.inc_x = 0;
					player1.inc_y = 1;
				}
				break;
		}

		if (analyseSituation()) {
			return;
		}
	}
	clear_screen();
}

int analyseSituation() {
	if (checkWin(player1.x, player1.y, player1.name, player2.x, player2.y, player2.name) == 1) {
		return 1;
	}

	board[player1.x][player1.y] = 1;
	board[player2.x][player2.y] = 1;

	player1.x += SQUARE_SIZE * player1.inc_x;
	player2.x += SQUARE_SIZE * player2.inc_x;
	player1.y += SQUARE_SIZE * player1.inc_y;
	player2.y += SQUARE_SIZE * player2.inc_y;

	draw_rectangle(player1.x, player1.y, SQUARE_SIZE, SQUARE_SIZE, PINK);
	draw_rectangle(player2.x, player2.y, SQUARE_SIZE, SQUARE_SIZE, LIGHT_GREEN);

	return 0;
}

int checkWin(uint16_t x1, uint16_t y1, char *name1, uint16_t x2, uint16_t y2, char *name2) {
	if (board[x1][y1] == 1 || OUT_OF_BOUNDS(x1, y1)) {
		make_sound(330, 3);
		clear_screen();
		printf("%s WINS\n", name2);
		player2.points++;
		return 1;
	}
	else if (board[x2][y2] == 1 || OUT_OF_BOUNDS(x2, y2)) {
		make_sound(330, 3);
		clear_screen();
		printf("%s WINS\n", name1);
		player1.points++;
		return 1;
	}
	else if (x2 == x1 && y2 == y1) {
		make_sound(330, 3);
		clear_screen();
		printf("It's a tie\n");
		return 1;
	}
	return 0;
}

void set_enviroment() {
	player1.x = SCREEN_WIDTH / 2;
	player1.y = SCREEN_HEIGHT * STARTING_OFFSET_1;
	player2.x = SCREEN_WIDTH / 2;
	player2.y = SCREEN_HEIGHT * STARTING_OFFSET_2;

	player1.inc_x = 0;
	player1.inc_y = 1;

	player2.inc_x = 0;
	player2.inc_y = -1;

	for (int i = 0; i < (SCREEN_WIDTH); i++) {
		for (int j = 0; j < (SCREEN_HEIGHT); j++) {
			board[i][j] = 0;
		}
	}

	for (int i = 0; i < (SCREEN_WIDTH); i++) {
		for (int j = 0; j < (SCREEN_HEIGHT * 0.04); j++) {
			board[i][j] = 1;
		}
	}
	clearscreen();
	draw_rectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT * 0.04, RED);
	printf(POINTS, player1.name, player1.points, player2.name, player2.points);
}