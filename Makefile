CC = gcc
CFLAGS = -Wall -Wextra -pthread
TARGET = dining_philosophers
SOURCE = src/dining_philosophers.c

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) $(SOURCE) -o $(TARGET)

run1: $(TARGET)
	./$(TARGET) 1

run2: $(TARGET)
	./$(TARGET) 2

run3: $(TARGET)
	./$(TARGET) 3

clean:
	rm -f $(TARGET)

.PHONY: all run1 run2 run3 clean
