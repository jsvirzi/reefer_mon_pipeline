## Installing ROOT from source

To install CERN ROOT on Ubuntu 14.04, it has been advised to use at least the following version.
Previous versions do not compile, at least without grief

    cd ${HOME}/projects
    
    wget root.cern.ch/download/root_v5.34.23.source.tar.gz
    
    tar xzvf root.cern.ch/download/root_v5.34.23.source.tar.gz
    
    ./configure --disable-x11
    
    make

The following is a shortcut to what the recommended setup.sh script that ROOT provides. 
This has worked for me for years, and will most likely stop working at some point in the future.
However, the future is tomorrow, not today, so here goes. 
Place the following lines of code somewhere in a startup script if you want to use ROOT

    export ROOTSYS=${HOME}/projects/root
    export PATH=${ROOTSYS}/bin:${PATH}
    export LD_LIBRARY_PATH=${ROOTSYS}/lib:${LD_LIBRARY_PATH}

To use ROOT in your project, add the following lines in your Makefile 
(in addition to other include and lib directives)

    ROOTINCS = -I$(shell root-config --incdir)
    ROOTLIBS := $(shell root-config --libs) $(shell root-config --auxlibs)
    
## Installing WiringPi

## Installing Cassandra

## Installing Kafka

## Installing BCM2835 GPIO Library
