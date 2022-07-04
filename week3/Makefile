PC=pkg-config opencv4 --cflags --libs
ARGS=-std=c++11 `$(PC)`
FILE=eval basename `pwd`
FILENAME = dip03.cpp
OUT=a.out
GPP=g++

BUILD=$(GPP) $(FILENAME) -o $(OUT) $(ARGS)

all:
	make run
build:
	$(BUILD)
run:
	$(BUILD)
	./$(OUT)

exec:
	./$(OUT)