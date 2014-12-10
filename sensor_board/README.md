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
    sudo losetup -f --show test.img
    
The (example) output of the previous command:

    /dev/loop0

That argument is used in the following commands.
It could be "/dev/loop1" for example.

    sudo fdisk -l /dev/loop0
    
You should be looking at a screen that looks something like this:

    Disk /dev/loop0: 16.0 GB, 16009658368 bytes
    255 heads, 63 sectors/track, 1946 cylinders, total 31268864 sectors
    Units = sectors of 1 * 512 = 512 bytes
    Sector size (logical/physical): 512 bytes / 512 bytes
    I/O size (minimum/optimal): 512 bytes / 512 bytes
    Disk identifier: 0x000b5098

      Device Boot      Start         End      Blocks   Id  System
    /dev/loop0p1            8192      122879       57344    c  W95 FAT32 (LBA)
    /dev/loop0p2          122880    31268863    15572992   83  Linux



    sudo fdisk -l /dev/mmcblk0
    sudo losetup -f --show -o 62914560 test.img /dev/loop2
    sudo e2fsck -f /dev/loop2

