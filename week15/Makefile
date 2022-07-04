PC=pkg-config opencv4 --cflags --libs
ARGS=-std=c++11 `$(PC)`
FILE=dip15.cpp
OUT=a.out
GPP=g++

BUILD=$(GPP) $(FILE) -o $(OUT) $(ARGS)
GL=g++ $(FILE) -framework OpenGL -framework GLUT -Wno-deprecated -o $(OUT) $(ARGS)

all: gl

gl:
	$(GL)
	./$(OUT)

gl/build:
	$(GL)

build:
	$(BUILD)
run:
	$(BUILD)
	./$(OUT)

exec:
	./$(OUT)