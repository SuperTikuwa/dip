PC=pkg-config opencv4 --cflags --libs
ARGS=-std=c++11 `$(PC)`
FILE=main.cpp
OUT=a.out
GPP=g++

BUILD=$(GPP) $(FILE) -o $(OUT) $(ARGS)
GL=g++ dip11_gl.cpp -framework OpenGL -framework GLUT -Wno-deprecated -o $(OUT) $(ARGS)

all: run

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