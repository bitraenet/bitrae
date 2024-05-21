#!/bin/bash

LOGFILE="$HOME/bitrae_install.log"  # Changed to use $HOME
REPO_URL="https://github.com/bitraenet/bitrae"
INSTALL_DIR="$HOME/bitrae"  # Install in the user's home directory

# Function to print in color
echo_color() {
    local color=$1
    local message=$2
    echo -e "\033[${color}m${message}\033[0m" | tee -a "$LOGFILE"
}

# Function to check the status of the last executed command
check_status() {
    if [ $? -ne 0 ]; then
        echo_color "0;31" "Error encountered. Check log file at $LOGFILE. Exiting..."
        exit 1
    fi
}

# Update and Install dependencies with retries
install_dependencies() {
    local max_attempts=3
    local attempt=1
    while [ $attempt -le $max_attempts ]; do
        echo_color "0;34" "Attempt $attempt: Installing dependencies..."
        sudo apt-get update && sudo apt-get upgrade && sudo apt-get install -y git build-essential libtool autotools-dev automake pkg-config bsdmainutils python3 libssl-dev libevent-dev libboost-system-dev libboost-filesystem-dev libboost-test-dev libboost-thread-dev libfmt-dev libsqlite3-dev libminiupnpc-dev libzmq3-dev libqt5gui5 libqt5core5a libqt5dbus5 qttools5-dev qttools5-dev-tools libqrencode-dev && break
        echo_color "0;33" "Attempt $attempt failed. Retrying..."
        sleep 5
        ((attempt++))
    done

    if [ $attempt -gt $max_attempts ]; then
        echo_color "0;31" "Failed to install dependencies after $max_attempts attempts."
        exit 1
    fi
}

echo_color "0;32" "Starting Bitrae Node installation..."
install_dependencies
check_status

# Cloning Bitrae Node repository
if [ -d "$INSTALL_DIR" ]; then
    echo_color "0;33" "Installation directory already exists. Skipping clone..."
else
    echo_color "0;34" "Cloning Bitrae Node repository"
    git clone $REPO_URL $INSTALL_DIR
    check_status
fi

# Change directory to INSTALL_DIR
echo_color "0;34" "Change directory to INSTALL_DIR"
cd "$INSTALL_DIR"
check_status

# Install BerkeleyDB
echo_color "0;34" "Installing BerkeleyDB"
./contrib/install_db4.sh $(pwd)
check_status

# Running autogen.sh
echo_color "0;34" "Preparing build configuration"
./autogen.sh
check_status

# Setting environment variable for BerkeleyDB
echo_color "0;34" "Setting BerkeleyDB Environment"
export BDB_PREFIX="$INSTALL_DIR/db4"
check_status

# Checking environment variables and directory contents
echo_color "0;34" "Checking environment variables"
ls "$BDB_PREFIX/lib"
check_status

# Setting Make configuration
echo_color "0;34" "Setting Make configuration"
./configure BDB_LIBS="-L${BDB_PREFIX}/lib -ldb_cxx-4.8" BDB_CFLAGS="-I${BDB_PREFIX}/include" --without-gui
check_status

# Compile the binary
echo_color "0;32" "Creating Bitrae binary"
make
check_status

# Final completion message
echo_color "0;32" "Bitrae Node install complete. Run command ./bitraed in $INSTALL_DIR/src to start Node, use flag -testnet to run node in TESTNET"
