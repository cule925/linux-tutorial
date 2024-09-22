#!/bin/bash

# Check if script is being executed as root
if [[ $EUID -ne 0 ]]; then
	echo "You must be root to do this." 1>&2
	exit 1
fi

# Store Ethernet interface name, bridge interface name and bridge interface MAC address in variables
ETH_IF=$(cat ethernet.txt)
BR_IF=$(cat bridge.txt)
BR_IF_MAC=$(cat bridge_mac.txt)

# Check if variables are empty
if [[ -z $ETH_IF ]]; then
	echo "No Ethernet interface name specified." 1>&2
	exit 1
fi
if [[ -z $BR_IF ]]; then
	echo "No bridge interface name specified." 1>&2
	exit 1
fi
if [[ -z $BR_IF_MAC ]]; then
	echo "No bridge interface MAC specified." 1>&2
	exit 1
fi

# Configure the network
ip link add name $BR_IF type bridge
ip link set $BR_IF address $BR_IF_MAC
ip link set $BR_IF up

ip link set $ETH_IF up
ip link set $ETH_IF master $BR_IF

sudo dhclient $BR_IF

echo "Bridge $BR_IF created!"
