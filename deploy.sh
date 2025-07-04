#!/bin/bash
echo "deploying"

DEST_IP="$1"
BINARY="$2"
DEST_DIR="/home/root"



# kill gdbserver on remote and delete old binary
ssh root@${DEST_IP} "sh -c '/usr/bin/killall -q gdbserver; rm -rf ${DEST_DIR}/${BINARY}  exit 0'"

# send binary to target
scp ../build/${BINARY} root@${DEST_IP}:${DEST_DIR}/${BINARY}
#scp -r ../resources root@${DEST_IP}:${DEST_DIR}/

echo "deployed"

