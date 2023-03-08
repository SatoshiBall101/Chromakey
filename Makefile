CC = g++
CFLAGS = -g -Wall
SRCS =  chromakey.cpp bmplib.cpp
TARGET = chromakey

all: chromakey

chromakey: chromakey.o bmplib.o
	$(CC) $(CFLAGS) chromakey.o bmplib.o -o chromakey

chromakey.o: chromakey.cpp bmplib.h
	$(CC) $(CFLAGS) -c chromakey.cpp

bmplib.o: bmplib.cpp bmplib.h
	$(CC) $(CFLAGS) -c bmplib.cpp

clean:
	rm -f chromakey.o chromakey sol-*.bmp stu-*.bmp o1.bmp o2.bmp *~
