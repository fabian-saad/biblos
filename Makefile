CC = gcc

CFLAGS = -Wall -Wextra
LDLIBS = -lcjson

TARGET = biblos
SRC = main.c parser.c
OBJ = $(SRC:.c=.o)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJ)