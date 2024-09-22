#!/bin/bash

# Check if script is being executed as root
if [[ $EUID -ne 0 ]]; then
	echo "You must be root to do this." 1>&2
	exit 1
fi

# Store disk name in variable
DISK=$(cat target_disk.txt)

# Check if variable is empty
if [[ -z $DISK ]]; then
	echo "No disk name specified." 1>&2
	exit 1
fi

# Disk size
echo "Enter disk size:"
read DISK_SIZE

# Create virtual disk
qemu-img create -f qcow2 $DISK $DISK_SIZE

echo "Disk created!"

# Copy OVMF variable database
cp /usr/share/edk2/x64/OVMF_VARS.4m.fd .

echo "OVMF variable database copied!"
