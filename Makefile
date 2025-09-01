# Makefile for cmk

CC      := gcc
CFLAGS  := -O2 -std=c11
LDLIBS  := -lm

TARGET  := cmk_demo
SRC     := cmk_single.c cmk_math.c cmk_mesh.c
OBJ     := $(SRC:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@ $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ) $(TARGET)
