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
    
## Installing Cassandra

## Installing Kafka

## First steps to build a new working Raspberry Pi

    sudo apt-get update

    sudo apt-get install cvs
    sudo apt-get install make
    sudo apt-get install git
    sudo apt-get install gcc
    sudo apt-get install g++
    sudo apt-get install python2.7-dev

### installing pip

    sudo apt-get install python-setuptools
    sudo easy_install pip
    
### needed for the console

    sudo pip install netifaces
    
### Installing I2C

Most of these instructions are taken from the [Adafruit page](https://learn.adafruit.com/adafruit-16x2-character-lcd-plus-keypad-for-raspberry-pi/usage), 
but are shown here to have them in one place

First, one must edit "/etc/modules" and add the following lines at the end to enable the i2c driver

    i2c-bcm2708
    i2c-dev

The driver will be enabled upon subsequent reboot of the system.
Continue without rebooting the remaining steps, 
keeping in mind that a reboot will be necessary before using the new system.

Continuing on...

    sudo apt-get install python-smbus
    sudo apt-get install i2c-tools

More commands, and some of them overlap with the previous steps.
They are kept here because they were copied/pasted from another site.

    sudo apt-get install build-essential python-dev python-smbus python-pip git
    sudo pip install RPi.GPIO
    mkdir ${HOME}/projects
    cd ${HOME}/projects
    git clone https://github.com/adafruit/Adafruit_Python_CharLCD.git
    cd Adafruit_Python_CharLCD
    sudo python setup.py install

### Installing WiringPi

    # mkdir -p ${HOME}/projects
    cd ${HOME}/projects
    git clone git://git.drogon.net/wiringPi
    cd wiringPi
    ./build

## Installing BCM2835 GPIO Library

    # mkdir -p ${HOME}/projects
    cd ${HOME}/projects
    wget http://www.airspayce.com/mikem/bcm2835/bcm2835-1.37.tar.gz
    tar -xzvf bcm2835-1.37.tar.gz
    cd bcm2835-1.37
    ./configure
    make
    sudo make check
    sudo make install

### Tether iPhone Onto Raspberry Pi

These instructions are line-by-line instructions to tether the Raspberry Pi to the iPhone.
They are stated here in stand-alone format,
but are intended to be executed after the above section(s) for installing a new Raspberry Pi.

After burning the image of Wheezy Raspbian onto the Raspberry, 
and booting up with an internet connection,
run the following command to update all installed packages

    sudo apt-get update
    
Then, to install the necessary packages to tether the iPhone to the Raspberry Pi,
perform the following commands (taken directly from Dave Conroy's [page](http://www.daveconroy.com/how-to-tether-your-raspberry-pi-with-your-iphone-5/))

    sudo apt-get install gvfs ipheth-utils
    sudo apt-get install libimobiledevice-utils gvfs-backends gvfs-bin gvfs-fuse
    sudo apt-get install ifuse
    sudo mkdir /media/iPhone
    
Thereafter, run

    ifconfig -s
    
to see the network devices that are connected. 
Proceed to connect the iPhone to the USB port,
and reboot the Raspberry Pi

    sudo reboot
    
Run the "ifconfig -s" command again to see where the operating system placed the iPhone.
For the sake of argument, let's assume the OS placed the iPhone as "eth1".
Edit the file "/etc/network/interfaces", and add the following lines:

    allow-hotplug eth1
    iface eth1 inet dhcp

Disconnect the ethernet cable (for eth0) and reboot the system.
In principle one could restart the network,
but this works for me and I haven't had much luck with the network restart (sudo /etc/init.d/networking (re)start).

    sudo reboot
    
After rebooting,  and run the following commands:

    sudo ifuse /media/iPhone
    
The iPhone should be tethered.



