ROOT_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

DIRS = src demos/01_model_loading

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
