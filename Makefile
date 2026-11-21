CC = gcc
CFLAGS = -Wall -Wextra -Iinclude

SOURCES = src/main.c src/server.c src/http.c src/mime.c src/files.c
TARGET = minihttpd

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all clean
