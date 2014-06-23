PROJECT = libfragmic.so

SRC := $(wildcard *.cpp) 
OBJECTS = $(SRC:.cpp=.o)
CFLAGS  = -I../external/include -fPIC -Wall -pedantic -g -std=c++11 -DGLM_FORCE_RADIANS

all: $(PROJECT)

.cpp.o:
	g++ -c $(CFLAGS) $<

$(PROJECT): $(OBJECTS)
	g++ -shared -o $(PROJECT) $(OBJECTS) -L../external/lib/ -lm -lGLU -lGLEW -lGL -L/home/ms/git/SDL2/build/libSDL2.so.2.0.0 -lpthread -lassimp -lBulletCollision -lBulletDynamics 
	mv $(PROJECT) ../build/.

clean:
	rm -f $(OBJECTS) 

