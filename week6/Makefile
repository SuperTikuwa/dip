PC=pkg-config opencv4 --cflags --libs
ARGS=-std=c++11 `$(PC)`
FILE=dip06.cpp
FILEb=dip06b.cpp
OUT=a.out
GPP=g++

BUILD=$(GPP) $(FILE) -o $(OUT) $(ARGS)
BUILDB=$(GPP) $(FILEb) -o $(OUT) $(ARGS)

all: run

build:
	$(BUILD)
build/b:
	$(BUILDB)
run:
	$(BUILD)
	./$(OUT)
run/b:
	$(BUILDB)
	./$(OUT)
exec:
	./$(OUT)