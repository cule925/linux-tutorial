#!/bin/bash

# Check if script is being executed as root
if [[ $EUID -ne 0 ]]; then
	echo "You must be root to do this." 1>&2
	exit 1
fi

# Store Ethernet interface name, bridge interface name and bridge interface MAC address in variables
ETH_IF=$(cat ethernet.txt)
BR_IF=$(cat bridge.txt)

# Check if variables are empty
if [[ -z $ETH_IF ]]; then
	echo "No Ethernet interface name specified." 1>&2
	exit 1
fi
if [[ -z $BR_IF ]]; then
	echo "No bridge interface name specified." 1>&2
	exit 1
fi

# Dismantle the network
sudo ip link set $ETH_IF down
sudo ip link set $ETH_IF nomaster

sudo ip link set $BR_IF down
sudo ip link delete $BR_IF type bridge

sudo ip link set $ETH_IF up

echo "Bridge $BR_IF destroyed!"
