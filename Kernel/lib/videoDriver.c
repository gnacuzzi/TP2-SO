// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <videoDriver.h>
#include <stdint.h>
#include <font.h>
#include <lib.h>
#include <stdarg.h>
#include <stddef.h>
#define MAX_CHARS 256

static int CHAR_WIDTH = 9;
static int CHAR_HEIGHT = 16;

static int DEFAULT_LETTER_SIZE = 1;

static int strcpychar(char *dest, const char *origin, char limit);

struct vbe_mode_info_structure {
	uint16_t attributes;  // deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a
						  // linear frame buffer.
	uint8_t window_a;	  // deprecated
	uint8_t window_b;	  // deprecated
	uint16_t granularity; // deprecated; used while calculating bank numbers
	uint16_t window_size;
	uint16_t segment_a;
	uint16_t segment_b;
	uint32_t win_func_ptr; // deprecated; used to switch banks from protected mode without returning to real mode
	uint16_t pitch;		   // number of bytes per horizontal line
	uint16_t width;		   // width in pixels
	uint16_t height;	   // height in pixels
	uint8_t w_char;		   // unused...
	uint8_t y_char;		   // ...
	uint8_t planes;
	uint8_t bpp;   // bits per pixel in this mode
	uint8_t banks; // deprecated; total number of banks in this mode
	uint8_t memory_model;
	uint8_t bank_size; // deprecated; size of a bank, almost always 64 KB but may be 16 KB...
	uint8_t image_pages;
	uint8_t reserved0;

	uint8_t red_mask;
	uint8_t red_position;
	uint8_t green_mask;
	uint8_t green_position;
	uint8_t blue_mask;
	uint8_t blue_position;
	uint8_t reserved_mask;
	uint8_t reserved_position;
	uint8_t direct_color_attributes;

	uint32_t framebuffer; // physical address of the linear frame buffer; write here to draw to the screen
	uint32_t off_screen_mem_off;
	uint16_t off_screen_mem_size; // size of memory in the framebuffer but not being displayed on the screen
	uint8_t reserved1[206];
} __attribute__((packed));

typedef struct vbe_mode_info_structure *VBEInfoPtr;

VBEInfoPtr VBE_mode_info = (VBEInfoPtr) 0x0000000000005C00;

#define WIDTH VBE_mode_info->width
#define HEIGHT VBE_mode_info->height
uint16_t cursor_x = 0;
uint16_t cursor_y = 0;

Color penColor = {255, 255, 255}; // blanco
Color backColor = {0, 0, 0};	  // negro

static void *getPtrToPixel(uint16_t x, uint16_t y);

static void *getPtrToPixel(uint16_t x, uint16_t y) {
	return (void *) (VBE_mode_info->framebuffer + 3 * (x + (y * (uint64_t) VBE_mode_info->width)));
}

void draw_rect(int x, int y, int width, int height, Color color) {
	if (x >= VBE_mode_info->width || y >= VBE_mode_info->height)
		return;

	uint16_t maxWidth = VBE_mode_info->width - x;
	if (width > maxWidth)
		width = maxWidth;

	uint16_t maxHeight = VBE_mode_info->height - y;
	if (height > maxHeight)
		height = maxHeight;

	Color *ptr = (Color *) getPtrToPixel(x, y);
	unsigned int adv = VBE_mode_info->width - width;
	for (int i = 0; i < height; i++) {
		for (int c = 0; c < width; c++)
			*(ptr++) = color;
		ptr += adv;
	}
}

void clear_screen() {
	uint8_t *pos = (uint8_t *) ((uint64_t) VBE_mode_info->framebuffer);
	for (uint32_t len = 3 * (uint32_t) VBE_mode_info->width * VBE_mode_info->height; len; len--, pos++) {
		*pos = 0;
	}
	cursor_x = 0;
	cursor_y = 0;
}

void print_new_line(void) {
	cursor_x = 0;

	if (cursor_y + (2 * CHAR_HEIGHT * DEFAULT_LETTER_SIZE) <= VBE_mode_info->height) {
		cursor_y += CHAR_HEIGHT * DEFAULT_LETTER_SIZE;
	}
	else {
		void *dst = (void *) ((uint64_t) VBE_mode_info->framebuffer);
		void *src = (void *) (dst + 3 * (CHAR_HEIGHT * DEFAULT_LETTER_SIZE * (uint64_t) VBE_mode_info->width));
		uint64_t len =
			3 * ((uint64_t) VBE_mode_info->width * (VBE_mode_info->height - CHAR_HEIGHT * DEFAULT_LETTER_SIZE));
		memcpy(dst, src, len);
		memset(dst + len, 0, 3 * (uint64_t) VBE_mode_info->width * CHAR_HEIGHT * DEFAULT_LETTER_SIZE);
	}
}

void draw_char(char c) {
	if (c == '\n') {
		print_new_line();
		return;
	}
	if (c == '\b') { // Borrar el caracter anterior
		if (cursor_x < DEFAULT_LETTER_SIZE * CHAR_WIDTH && cursor_y > 0) {
			cursor_y -= CHAR_HEIGHT * DEFAULT_LETTER_SIZE;
			cursor_x = (VBE_mode_info->width / DEFAULT_LETTER_SIZE * CHAR_WIDTH) * DEFAULT_LETTER_SIZE * CHAR_WIDTH -
					   DEFAULT_LETTER_SIZE * CHAR_WIDTH;
		}
		else {
			cursor_x -= DEFAULT_LETTER_SIZE * CHAR_WIDTH;
		}
		draw_rect(cursor_x, cursor_y, DEFAULT_LETTER_SIZE * CHAR_WIDTH, CHAR_HEIGHT * DEFAULT_LETTER_SIZE, backColor);
		return;
	}
	if (c >= FIRST_CHAR && c <= LAST_CHAR) {
		const char *data = font + 32 * (c - FIRST_CHAR);
		for (int h = 0; h < 16; h++) {
			for (int w = 0; w < 8; w++) {
				if (*data & (1 << w)) {
					draw_rect(cursor_x + w * DEFAULT_LETTER_SIZE, cursor_y + h * DEFAULT_LETTER_SIZE,
							  DEFAULT_LETTER_SIZE, DEFAULT_LETTER_SIZE, penColor);
				}
			}
			data++;
			for (int w = 0; w < 8; w++) {
				if (*data & (1 << w)) {
					draw_rect(cursor_x + (w + 8) * DEFAULT_LETTER_SIZE, cursor_y + h * DEFAULT_LETTER_SIZE,
							  DEFAULT_LETTER_SIZE, DEFAULT_LETTER_SIZE, penColor);
				}
			}
			data++;
		}
	}

	cursor_x += DEFAULT_LETTER_SIZE * CHAR_WIDTH;
	if (cursor_x > VBE_mode_info->width - CHAR_WIDTH) {
		print_new_line();
	}
}

void set_lettersize(int size) {
	DEFAULT_LETTER_SIZE = size;
}

// lo mismo que hay en el main
static void va_printf(const char *fmt, va_list args);
static unsigned int log(uint64_t n, int base) {
	unsigned int count = 1;
	while (n /= base)
		count++;
	return count;
}

void printf(const char *fmt, ...) {
	va_list vl; // donde se guardan los argumentos variables
	va_start(vl, fmt);
	va_printf(fmt, vl);
	va_end(vl);
}

static void va_printf(const char *fmt, va_list args) {
	char buffer[MAX_CHARS] = {0};
	const char *aux = fmt; // puntero
	while (*aux) {
		if (*aux == '%') {
			aux++;
			int dx = strtoi(aux, &aux); // si es numero lo devuelve en formato decimal sino develve 0
			int len;

			switch (*aux) {
				case 'c': // es un char
					putchar(va_arg(args, int));
					break;

				case 'd':									   // es un entero
					len = itoa(va_arg(args, int), buffer, 10); // esta en base 10
					printchars('0', dx - len);
					puts(buffer);
					break;

				case 'x':									   // hexadcimal
					len = itoa(va_arg(args, int), buffer, 16); // esta en base 16
					printchars('0', dx - len);
					puts(buffer);
					break;

				case 's': // es un string
					printchars(' ', dx);
					puts(va_arg(args, char *));
					break;
			}
		}
		else {
			putchar(*aux);
		}
		aux++;
	}
}

void putchar(char c) {
	draw_char(c);
}

void puts(const char *s) {
	while (*s) {
		putchar(*s++);
	}
}

void printchars(char c, int n) {
	while (n >= 0) {
		putchar(c);
		n--;
	}
}

int itoa(int n, char *buffer, int base) {
	if (n == 0) {
		buffer[0] = '0';
		buffer[1] = '\0';
		return 1;
	}

	unsigned int len = 0;
	int i = 0;
	if (n < 0 && base == 10) {
		n = -n;
		buffer[i] = '-';
		len++;
		i++;
	}

	len += log(n, base);
	while (n != 0) {
		int res = n % base;
		buffer[len - i++ - 1] = (res > 9) ? (res - 10) + 'A' : res + '0';
		n /= base;
	}
	buffer[i] = '\0';
	return len;
}
int strtoi(const char *s, const char **end) {
	int n = 0;
	while (isnumber(*s)) {
		n = n * 10 + (*s++) - '0';
	}
	*end = s;
	return n;
}
int isnumber(int n) {
	return ((n) >= '0' && (n) <= '9');
}

int strlen(const char *str) {
	int len = 0;
	while (*(str++) != '\0')
		len++;
	return len;
}

int arrLen(char **array) {
	int len = 0;
	while (*(array++) != NULL)
		len++;
	return len;
}

int strcpy(char *dest, const char *origin) {
	return strcpychar(dest, origin, '\0');
}

static int strcpychar(char *dest, const char *origin, char limit) {
	int idx = 0;
	while (origin[idx] != limit && origin[idx] != '\0') {
		dest[idx] = origin[idx];
		idx++;
	}
	dest[idx] = 0;
	return idx;
}
