#ifndef VIDEO_DRIVER_H
#define VIDEO_DRIVER_H

#include <stdint.h>
#define BLACK 0x000000
#define WHITE 0xFFFFFF

typedef struct {
	uint8_t b;
	uint8_t g;
	uint8_t r;
} Color;

typedef union {
	Color color;
	uint32_t bits;
} ColorInt;

void putPixel(Color color, uint64_t x, uint64_t y);
void draw_char(char c);
void draw_string(const char *str);
void draw_rect(int x, int y, int width, int height, Color color);
void clear_screen();
void print_new_line(void);
void set_lettersize(int size);

// para poder imprimir en el back
void printf(const char *fmt, ...);
void putchar(char c);
void puts(const char *s);
void printchars(char c, int n);
int strtoi(const char *s, const char **end);
int itoa(int n, char *buffer, int base);
int isnumber(int ch);
int strlen(const char *str);

#endif
