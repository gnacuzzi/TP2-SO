#ifndef COLOR_H
#define COLOR_H

typedef struct {
	uint8_t b;
	uint8_t g;
	uint8_t r;
} Color;

typedef union {
	Color color;
	uint32_t bits;
} ColorInt;

#define SILVER ((Color) {173, 169, 170})
#define RED ((Color) {0, 0, 255})
#define LIGHT_GREEN ((Color) {100, 255, 100})
#define DARK_GREEN ((Color) {100, 150, 100})
#define PINK ((Color) {255, 153, 184})

#endif