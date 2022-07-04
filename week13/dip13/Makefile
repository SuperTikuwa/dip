PC=pkg-config opencv4 --cflags --libs
ARGS=-std=c++11 `$(PC)`
FILE=dip13a.cpp
FILEB=dip13b.cpp

OUT=a.out
GPP=g++

BUILD=$(GPP) $(FILE) -o $(OUT) $(ARGS)
GL=g++ $(FILE) -framework OpenGL -framework GLUT -Wno-deprecated -o $(OUT) $(ARGS)
GLB=g++ $(FILEB) -framework OpenGL -framework GLUT -Wno-deprecated -o $(OUT) $(ARGS)

.PHONY: gl/a
gl/a:
	$(GL)
	./$(OUT)

gl/b:
	$(GLB)
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