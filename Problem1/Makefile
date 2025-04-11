CC = gcc
CFLAGS = -Wall -std=c11
OBJ = seqlock.o
TARGET = seqlock

all: $(TARGET)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

seqlock.o: seqlock.c seqlock.h
	$(CC) $(CFLAGS) -c seqlock.c

clean:
	rm -f $(OBJ) $(TARGET)