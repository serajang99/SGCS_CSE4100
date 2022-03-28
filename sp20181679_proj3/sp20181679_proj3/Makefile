CC = gcc
CFLAGS = -W -Wall
TARGET = 20181679.out
SRCS = 20181679.c
OBJECTS = 20181679.o

$(TARGET):$(OBJECTS)
	$(CC) -o $(TARGET) $(OBJECTS) $(CFLAGS)

$(OBJECTS): 20181679.c 
	$(CC) -c 20181679.c $(CFLAGS)

clean:
	rm -rf *.o *.out
