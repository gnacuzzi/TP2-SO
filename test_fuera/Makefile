CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -DNO_MALLOC
LDFLAGS = 
SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = test_fuera_mm

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE) 

.PHONY: all clean