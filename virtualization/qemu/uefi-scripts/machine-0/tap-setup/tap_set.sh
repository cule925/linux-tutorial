#!/bin/bash

# Check if script is being executed as root
if [[ $EUID -ne 0 ]]; then
	echo "You must be root to do this." 1>&2
	exit 1
fi

# Store bridge interface name, TAP interface name and TAP interface MAC address in variables
BR_IF=$(cat ../../bridge.txt)
TAP_IF=$(cat tap.txt)
TAP_IF_MAC=$(cat tap_mac.txt)

# Check if variables are empty
if [[ -z $BR_IF ]]; then
	echo "No bridge interface name specified." 1>&2
	exit 1
fi
if [[ -z $TAP_IF ]]; then
	echo "No TAP interface name specified." 1>&2
	exit 1
fi
if [[ -z $TAP_IF_MAC ]]; then
	echo "No TAP interface MAC specified." 1>&2
	exit 1
fi

ip tuntap add $TAP_IF mode tap user root
ip link set $TAP_IF address $TAP_IF_MAC

ip link set $TAP_IF master $BR_IF
ip link set $TAP_IF up

echo "TAP interface $TAP_IF created!"
