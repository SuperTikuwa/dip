PC=pkg-config opencv4 --cflags --libs
ARGS=-std=c++11 `$(PC)`
FILE=dip04.cpp
OUT=a.out
GPP=g++

BUILD=$(GPP) $(FILE) -o $(OUT) $(ARGS)

all:
	make run

build:
	$(BUILD)
run:
	$(BUILD)
	./$(OUT)

exec:
	./$(OUT)