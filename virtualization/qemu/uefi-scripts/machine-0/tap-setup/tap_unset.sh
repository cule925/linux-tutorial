#!/bin/bash

# Check if script is being executed as root
if [[ $EUID -ne 0 ]]; then
	echo "You must be root to do this." 1>&2
	exit 1
fi

# Store TAP interface name in variable
TAP_IF=$(cat tap.txt)

# Check if variable is empty
if [[ -z $TAP_IF ]]; then
	echo "No TAP interface name specified." 1>&2
	exit 1
fi

ip link set $TAP_IF down
ip link set $TAP_IF nomaster

ip tuntap del $TAP_IF mode tap

echo "TAP interface $TAP_IF destroyed!"
