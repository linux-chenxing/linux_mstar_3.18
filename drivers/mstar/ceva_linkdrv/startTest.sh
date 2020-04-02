#!/bin/bash

# There's no point of running this script without sudo
if [[ $EUID -ne 0 ]]; then
   echo "This script must be run as root" 1>&2
   exit 1
fi

# Set verbosity of printk to highest
echo 8 > /proc/sys/kernel/printk

insmod cevalink.ko
./test &
./test &
./test &
./test &
rmmod cevalink.ko
