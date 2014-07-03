ROOT_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

DIRS = src demos/01_model_loading demos/02_physics_simple demos/03_skeletal_collision demos/04_normal_mapping demos/05_game_demo

all : libfragmic.so

libfragmic.so : force_look
	-@for d in $(DIRS); do (cd $$d; $(MAKE) $(MFLAGS) ); done

clean :
	-@for d in $(DIRS); do (cd $$d; $(MAKE) clean ); done
	-@rm build/*.so

check :
	-@for d in $(DIRS); do (cppcheck --enable=all --std=posix --std=c++11 $$d ); done

force_look :
	-@true
