#!/bin/bash

# Send the UDP Message with NetCat
printf "Sending UDP message"
NETWORK_INTERFACE_NAME=$(route | grep '^default' | grep -o '[^ ]*$')
NETWORK_INTERFACE_ADDRESS=$(ip addr show $NETWORK_INTERFACE_NAME | grep -Po 'inet \K[\d.]+')
printf $NETWORK_INTERFACE_NAME
printf $NETWORK_INTERFACE_ADDRESS
cat udp_message_upd.json | nc -u -w 3 $NETWORK_INTERFACE_ADDRESS 8762
exit 0
