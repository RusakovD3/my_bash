CC := gcc
CFLAGS := -Wall -Werror -Wextra
SOURCES := main.c m_bash_source.c
OBJECTS := $(SOURCES:.c=.o)
TARGET := my_bash

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean
