PROJECTROOT=${HOME}/projects/reefer_mon
INCLUDES=-I$(PROJECTROOT)/include -I${HOME}/projects/librdkafka/src
SYSLIBS = -lg2c -lm -ldl
LIBS =  -lz -lpthread -lrt -L${HOME}/projects/librdkafka/src -lrdkafka

all : bin/kafka_interface

CFLAGS = -fPIC -Wall -c -g $(shell root-config --cflags) 
LFLAGS = 

bin/kafka_interface : src/kafka_interface.c
	gcc src/kafka_interface.c ${HOME}/projects/librdkafka/src/librdkafka.a $(INCLUDES) $(LIBS) -o bin/kafka_interface

clean:
	rm -f bin/kafka_interface 

