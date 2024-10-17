// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <libc.h>
#include <syscall.h>

#include <stdarg.h>
#define MAX_CHARS 256

#define IS_ALPHA(c) (((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z'))
#define IS_DIGIT(c) ((c) <= '9' && (c) >= '0')
#define TO_UPPER(c) ((c) - (((c) >= 'a' && (c) <= 'z') ? 'a'-'A' : 0))
#define VALID_NUM_FOR_BASE(c, base) (((base) <= 10 && ((c) >= '0' && (c) < '0' + ((base) <= 10 ? (base) : 10))) || ((base) > 10 && (IS_DIGIT(c) || (((c) >= 'a' && (c) < 'a' + ((base) - 10)) || ((c) >= 'A' && (c) < 'A' + ((base) - 10))))))
#define VALUE_OF(x) (TO_UPPER(x) - (IS_DIGIT(x) ? '0' : 55))

static void va_printf(const char *fmt, va_list args);

static unsigned int log(uint64_t n, int base) {
	unsigned int count = 1;
	while (n /= base)
		count++;
	return count;
}

static int isnumber(int n) {
	return ((n) >= '0' && (n) <= '9');
}

char readchar() {
	char localBuff[1];
	read_char(localBuff);
	return localBuff[0];
}

// https://www.equestionanswers.com/c/c-printf-scanf-working-principle.php
// usamos esta biblioteca para el manejo de argumentos variables

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
	write(1, c);
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

int strcmp(const char *s1, const char *s2) {
	for (; *s1 == *s2 && *s1 != '\0'; s1++, s2++)
		;
	return *s1 - *s2;
}

bool isNumber(char ch) {
	return ch >= '0' && ch <= '9';
}

void clearscreen() {
	clear_screen();
}

int ctoi(char s) {
	int num = 0;
	if (isNumber(s)) {
		num = num * 10 + s - '0';
	}
	return num;
}

int scanf(char *buffer) {
	int idx = 0;
	while (1) {
		char c = readchar();
		if (c != -1 && c != 0) {
			if (c == '\b') {
				if (idx > 0) {
					putchar(c);
					idx--;
				}
			}
			else if (c == '\n') {
				printf("\n");
				buffer[idx] = 0;
				if (buffer[0] != 0) {
					return 1;
				}
				return 0;
			}
			else if (c != '\t') {
				buffer[idx++] = c;
				putchar(c);
			}
		}
	}
	return -1;
}

int atoi(char *str) {
	int res = 0;
	for (int i = 0; str[i] != '\0'; ++i)
		res = res * 10 + str[i] - '0';

	return res;
}

int strtol(const char *str, const char **endptr, int base) {
    if (base != 0 && ((base < 2 || base > 36) || str[0] == '\0' || !VALID_NUM_FOR_BASE(str[0], base))) {
        return -1;
    }

    int digit;
    int64_t number = 0;
    int negative = 0;
    int finished = 0;
    int i = 0;

    while (str[i] == ' ' || str[i] == '+'){
		i++;
	}

    if (str[i] == '-') {
        negative = 1;
        i++;
    }

    if (base == 16 && str[i] == '0' && (str[i + 1] == 'x' || str[i + 1] == 'X')){
		i += 2;
	}

    if (base == 8 && str[i] == '0'){
		i++;
	}

    if (base == 0) {
        if (str[i] == '0' && (str[i + 1] == 'x' || str[i + 1] == 'X')){
			base = 16;
		} else if (str[i] == '0') {
			base = 8;
		} else {
			base = 10;
		}
        i += base == 16 ? 2 : (base == 8 ? 1 : 0);
    }

    for (; str[i] != '\0' && !finished; i++) {
        digit = -1;
        if (VALID_NUM_FOR_BASE(str[i], base)){
			digit = VALUE_OF(str[i]);
		}
            
        if (digit < base && digit != -1){
			number = number * base + digit * (negative ? -1 : 1);
		} else {
			finished = 1;
		}
    }
	if (!IS_DIGIT(str[i - 1]) && !IS_ALPHA(str[i - 1])){
		i = 0;
	}
    if (finished){
		i--;
	}
    if (endptr){
		*endptr = &str[i];
	}
    return number;
}
