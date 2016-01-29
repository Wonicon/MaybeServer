CC := gcc
CFLAGS := -std=c99 -Wall -Werror -I ./include

TARGET := server
SRC_DIR := ./src

CFILES := $(shell find $(SRC_DIR)/ -name "*.c")
OBJS := $(CFILES:%.c=%.o)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean

clean:
	@rm -f $(OBJS)
	@rm -f $(TARGET)
