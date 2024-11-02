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
    sysRead(0, localBuff, 1);
	return localBuff[0];
}

// https://www.equestionanswers.com/c/c-printf-scanf-working-principle.php
// usamos esta biblioteca para el manejo de argumentos variables


void printf(const char *fmt, ...) {
    va_list vl; // Donde se guardan los argumentos variables
    va_start(vl, fmt);
    va_printf(fmt, vl);
    va_end(vl);
}

static void va_printf(const char *fmt, va_list args) {
    char buffer[MAX_CHARS] = {0};
    const char *aux = fmt; // Puntero

    while (*aux) {
        if (*aux == '%') {
            aux++;
            int dx = strtoi(aux, &aux); // Si es número lo devuelve en formato decimal, sino devuelve 0
            int len;

            switch (*aux) {
                case 'c': // Es un char
                    putchar(va_arg(args, int)); // `char` se pasa como `int`
                    break;

                case 'd': // Es un entero
                    len = itoa(va_arg(args, int), buffer, 10); // Está en base 10
                    printchars('0', dx - len);
                    puts(buffer);
                    break;

                case 'x': // Hexadecimal
                    // Cambiar a uint64_t para manejar 64 bits
                    len = itoa(va_arg(args, uint64_t), buffer, 16); // Está en base 16
                    printchars('0', dx - len);
                    puts(buffer);
                    break;

                case 's': // Es un string
                    printchars(' ', dx);
                    puts(va_arg(args, char *));
                    break;
            }
        } else {
            putchar(*aux);
        }
        aux++;
    }
}

void putchar(char c) {
	sysWrite(1, &c, 1);
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
        
        // Check for valid input
        if (c != -1 && c != 0) {
            // Handle backspace
            if (c == '\b') {
                if (idx > 0) {
                    idx--; // Decrement index
                    putchar(c); // Echo backspace
                    putchar(' '); // Clear the character
                    putchar(c); // Echo backspace again
                    buffer[idx] = '\0'; // Terminate string
                }
            }
            // Handle new line
            else if (c == '\n') {
                putchar('\n'); // Echo new line
                buffer[idx] = '\0'; // Terminate string
                if (idx > 0) { // Check if something was entered
                    return 1; // Return success
                }
                return 0; // Return failure (no input)
            }
            // Ignore tab characters
            else if (c != '\t') {
                buffer[idx++] = c; // Store character in buffer
                putchar(c); // Echo the character
                buffer[idx] = '\0'; // Terminate string
            }

            // Optional: Check buffer overflow
            if (idx >= BUFFER_SIZE) { // Assuming BUFFER_SIZE is defined
                printf("\nBuffer full\n");
                buffer[idx] = '\0'; // Ensure null-termination
                return 1; // Consider it valid but cut-off input
            }
        }
    }
    return -1; // Should never reach here
}

int atoi(char *str) {
	int res = 0;
	for (int i = 0; str[i] != '\0'; ++i)
		if (IS_DIGIT(str[i]))
			res = res * 10 + str[i] - '0';

	return res;
}

int scanLine(char *buffer, int maxLen) {
    int idx = 0;
    char c;

    while (1) {
        c = readchar();  // Leer un carácter

        // Verificar si se recibió EOF (-1)
        if (c == -1) {
            return -1;  // Retornar -1 indicando EOF
        }

        // Si se encuentra un '\n', imprimir la línea y terminar
        if (c == '\n') {
            buffer[idx] = '\0';  // Terminar la cadena
            return 0;  // Retornar éxito
        }

        // Almacenar el carácter en el buffer si no se excede el límite
        if (idx < maxLen - 1) {
            buffer[idx++] = c;
        }
    }
}