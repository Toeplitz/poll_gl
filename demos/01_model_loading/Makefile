PROJECT = 01_model_loading

SRC := $(wildcard *.cpp)
OBJECTS = $(SRC:.cpp=.o)
CFLAGS  = -I../../external/include -Wall -pedantic -g -std=c++11

all: $(PROJECT)

.cpp.o:
	g++ -c -I../../src/ $(CFLAGS) $<

$(PROJECT): $(OBJECTS)
	g++ -o $(PROJECT) $(OBJECTS) -L../../external/lib/ -L../../build/ -lfragmic -lassimp -lGLU -lGLEW -lGL -lSDL2

clean:
	rm -f $(OBJECTS) $(PROJECT)



