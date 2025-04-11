CC = gcc
CFLAGS = -std=c11
OBJ = problem4.o
TARGET = problem4

all: $(TARGET)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)
problem4.o: problem4.c
	$(CC) $(CFLAGS) -c problem4.c

clean:
	rm -f $(OBJ) $(TARGET)