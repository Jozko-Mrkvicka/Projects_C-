CPP  = g++.exe
SHELL = cmd

LIB_PATHS = -L "../Common/External/WinBGIm/"

LIBS =  -lbgi \
		-luuid \
		-loleaut32 \
		-lole32

INCS =  -I "../Common/External/WinBGIm/" \
		-I "inc/"

SRC = src

FLAGS = -mwindows

BIN = bin


all: clean cube.exe

clean:
	rm -f \
		*.exe \
		$(BIN)/*.o \
		$(BIN)/*.exe

cube.exe: cube.o cube_dat.o
	g++ $(BIN)/cube.o $(BIN)/cube_dat.o $(LIB_PATHS) $(LIBS) $(FLAGS) -o cube.exe

cube_dat.o:
	g++ -c $(SRC)/cube_dat.c $(INCS) -o $(BIN)/cube_dat.o

cube.o:
	g++ -c $(SRC)/cube.c $(INCS) -o $(BIN)/cube.o

