CC = gcc
CFLAGS = -std=c11
OBJ = logbuf.o
TARGET = logbuf

all: $(TARGET)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)
logbuf.o: logbuf.c
	$(CC) $(CFLAGS) -c logbuf.c

clean:
	rm -f $(OBJ) $(TARGET)