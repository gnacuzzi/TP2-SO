CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -DNO_MALLOC
LDFLAGS = 

# Define cuál usar: buddy o bitmap
MEMORY_MANAGER = buddy  # Cambia a "bitmap" para usar bitmap.c

# Si MEMORY_MANAGER es "buddy", define la macro BUDDY; si es "bitmap", define BITMAP
ifeq ($(MEMORY_MANAGER), buddy)
    CFLAGS += -DBUDDY
    SOURCES = testMain.c test_mm.c test_util.c buddy.c
else ifeq ($(MEMORY_MANAGER), bitmap)
    CFLAGS += -DBITMAP
    SOURCES = testMain.c test_mm.c test_util.c bitmap.c
endif

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

