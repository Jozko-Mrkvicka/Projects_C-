CPP = g++.exe
GCC = gcc.exe
SHELL = cmd

LIB_PATHS = -L "../Common/External/WinBGIm/"

LIBS =  -lbgi      \
		-luuid     \
		-loleaut32 \
		-lole32

INCS = -I "../Common/External/WinBGIm/"

FLAGS = -mwindows


all: clean tank.exe

clean:
	rm -f *.o *.exe

tank.exe: tank.o
	g++ tank.o $(LIB_PATHS) $(LIBS) $(FLAGS) -o tank.exe

tank.o: tank.c
	g++ -c -g tank.c $(INCS)

