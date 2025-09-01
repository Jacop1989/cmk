# Makefile for cmk

CC      := gcc
CFLAGS  := -O2 -std=c11
LDLIBS  := -lm

# ไบนารีสำหรับเดโม
TARGET  := cmk_demo
SRC     := cmk_single.c cmk_math.c cmk_mesh.c cmk_prims.c cmk_stl.c cmk_obj.c
OBJ     := $(SRC:.c=.o)

# ไบนารีสำหรับเทสต์
TEST_TARGET := cmk_test
TEST_SRC    := cmk_test.c cmk_math.c cmk_mesh.c cmk_prims.c cmk_stl.c
TEST_OBJ    := $(TEST_SRC:.c=.o)

# รองรับ Windows ให้ใส่ .exe อัตโนมัติ
ifeq ($(OS),Windows_NT)
  EXE := .exe
else
  EXE :=
endif

TARGET_BIN := $(TARGET)$(EXE)
TEST_BIN   := $(TEST_TARGET)$(EXE)

.PHONY: all clean test

all: $(TARGET_BIN)

$(TARGET_BIN): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@ $(LDLIBS)

$(TEST_BIN): $(TEST_OBJ)
	$(CC) $(CFLAGS) $(TEST_OBJ) -o $@ $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# รันยูนิตเทสต์อย่างง่าย: สร้างและรันไบนารีเทสต์
test: $(TEST_BIN)
	./$(TEST_BIN)

clean:
	$(RM) $(OBJ) $(TEST_OBJ) $(TARGET_BIN) $(TEST_BIN) $(TARGET) $(TEST_TARGET)
