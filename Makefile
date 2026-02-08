CC = gcc
CFLAGS = -Iinclude -Wall -Wextra
LDFLAGS = -lm
SRC = src/main.c src/injector.c src/brain.c src/map_scanner.c
OBJ = $(SRC:.c=.o)
TARGET = build/linInject

all: $(TARGET)

$(TARGET): $(OBJ)
	@mkdir -p build
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o build/*
	rm -rf data/*.csv
