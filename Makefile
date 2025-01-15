CC = gcc
CFLAGS = -Wall -Wextra -g -std=c11
LDFLAGS = -lm

SRCS = main.c lookup.c
HEADERS = mlpt.h config.h

OBJS = $(SRCS:.c=.o)

TARGET = page_table

all: $(TARGET)

# Linking before compilation
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean
clean:
	rm -f $(OBJS) $(TARGET)

# Debug setting
debug: CFLAGS += -DDEBUG -O0
debug: all

# Run the target .o executable
run: $(TARGET)
	./$(TARGET)

test: $(TARGET)
	./$(TARGET)

.PHONY: all clean debug run test
