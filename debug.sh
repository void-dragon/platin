#!/bin/bash

cp Debug/Platin os/boot/platin
qemu-system-i386 -s -S -smp cores=2 -cpu core2duo -m 256M os.img
