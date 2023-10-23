
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

if [ -n "$1" ]; then
  TARGET=$1
else
  echo "Please add argument to define target. Usage: deploy_to_target.sh <user>@<ip-address>"
  exit
fi

LIB_NAME="libbsw-persistency.so.1.0.0"
SOURCE_DIR="$SCRIPT_DIR/.."
BUILD_DIR="$SCRIPT_DIR/../build"
echo "Deploy CCU $LIB_NAME to: $TARGET"

cd "$SCRIPT_DIR/.."

# Connect
SSHSOCKET=~/.ssh/$TARGET
ssh -M -f -N -o ControlPath=$SSHSOCKET $TARGET

# Copy Data
scp -o ControlPath=$SSHSOCKET $BUILD_DIR/$LIB_NAME $TARGET:/usr/lib

# Disconnect
ssh -S $SSHSOCKET -O exit $TARGET
