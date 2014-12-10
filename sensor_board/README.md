# Raspberry Pi Disk Image

These are step-by-step instructions for cloning an image of the Raspberry Pi.
Since not all 16GB are the same "size", you might encounter an image that is too large to flash onto another SD card,
presumably of the same size.
You will have to resize the partition.
Instead of mucking around on actual hardware, this can be done in software.
The operation is performed on an image of the disk, not on the hardware itself.
I am mainly taking this information from 
[RomanG](http://www.raspberrypi.org/forums/memberlist.php?mode=viewprofile&u=31378) 
who posted this on the Raspberry Pi forum(s).
Here is the [post](http://www.raspberrypi.org/forums/viewtopic.php?f=91&t=19468)
Assume the Raspberry Pi image is called "raspi.img".
This doesn't have to be a Raspberry Pi; it could be the image of any disk.

    dd if=/dev/sdb of=raspi.img 
    cp raspi.img test.img
    sudo losetup -f --show test.img /dev/loop1
    sudo fdisk -l /dev/loop1
    
You should be looking at a screen that looks something like this:

    sudo fdisk -l /dev/mmcblk0
    sudo losetup -f --show -o 62914560 test.img /dev/loop2
    sudo e2fsck -f /dev/loop2

