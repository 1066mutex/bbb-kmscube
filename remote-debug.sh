#!/bin/bash

echo "debugging"

DEST_IP="$1"
DEBUG_PORT="$2"
BINARY="$3"
DEST_DIR="/home/root"



# start gdbserver on target
# ssh -t root@${DEST_IP} "sh -c 'cd ${DEST_DIR}; gdbserver localhost:${DEBUG_PORT} ${BINARY} -i /tmp/fifo-in -o /tmp/fifo-out'"
ssh -t root@${DEST_IP} "sh -c 'cd ${DEST_DIR}; gdbserver localhost:${DEBUG_PORT} ${BINARY}'"
