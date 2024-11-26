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

# Set option
echo "Booting with ISO [y/n]?"
read BOOT_WITH_ISO

if [[ "$BOOT_WITH_ISO" == "y" || "$BOOT_WITH_ISO" == "Y" ]]; then
	
	# Store ISO absolute path
	ISO_PATH=$(cat iso_path.txt)
	
	# Check if variable is empty
	if [[ -z $ISO_PATH ]]; then
		echo "No ISO path specified." 1>&2
		exit 1
	fi
	
	# Set CDROM argument
	CDROM_ARG="-cdrom $ISO_PATH"

fi

# Create TAP interface
cd tap-setup/
./tap_set.sh
cd ../

# Get the TAP interface name
TAP_IF=$(cat tap-setup/tap.txt)

# Check if variable is empty
if [[ -z $TAP_IF ]]; then
	echo "No TAP interface name specified." 1>&2
	exit 1
fi

# Boot with disk
qemu-system-x86_64 \
-enable-kvm \
-m 8G \
-cpu host \
-smp 4 \
-netdev tap,id=net0,ifname=$TAP_IF,script=no,downscript=no \
-device virtio-net-pci,netdev=net0 \
$CDROM_ARG \
-drive format=qcow2,file=$DISK \
-audio driver=pa,model=virtio,server=/run/user/1000/pulse/native \
-vga virtio \
-drive if=pflash,format=raw,readonly=on,file=/usr/share/edk2/x64/OVMF_CODE.4m.fd \
-drive if=pflash,format=raw,file=OVMF_VARS.4m.fd

# Destroy TAP interface
cd tap-setup/
./tap_unset.sh
cd ../
