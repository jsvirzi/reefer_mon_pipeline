CFLAGS = -fPIC -Wall -c -g $(INCLUDES)

all: lib/libgps.so

lib/libgps.so : src/gps.c 
	g++ -shared -o lib/libgps.so -fPIC src/gps.c 
