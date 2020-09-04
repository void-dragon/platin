#!/bin/bash

cp Debug/Platin os/boot/platin
strip -s os/boot/platin
#qemu -smp cores=2 -cpu core2duo -m 512M -cdrom /media/Power-Silver/ISO/archlinux-2011.08.19-netinstall-i686.iso os.img 
qemu-system-i386 -smp cores=2 -cpu core2duo -m 256M os.img
