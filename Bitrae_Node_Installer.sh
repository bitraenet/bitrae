#!/bin/bash

LOGFILE="$HOME/bitrae_install.log"
REPO_URL="https://github.com/bitraenet/bitrae.git"
INSTALL_DIR="$HOME/bitrae"
TEMPFILE=$(mktemp)  # Temporary file to capture stderr
GIT_CLONE_MONITOR_INTERVAL=30  # Interval in seconds to check progress
GIT_CLONE_TIMEOUT=300  # Timeout if no progress is detected for 300 seconds
GIT_PID=""
MAX_NETWORK_RETRIES=5  # Maximum number of retries for network connectivity

# Function to print in color and log without color codes
echo_color() {
    local color=$1
    local message=$2
    local timestamp=$(date '+%Y-%m-%d %H:%M:%S')
    echo -e "\033[${color}m${timestamp} - ${message}\033[0m"
    echo "${timestamp} - $(echo -e "${message}" | sed -r 's/\x1b\[[0-9;]*m//g')" >> "$LOGFILE"
}

# Function to check the status of the last executed command
check_status() {
    local last_command=$(history | tail -n 3 | head -n 1)  # Capture the last executed command
    if [ $? -ne 0 ]; then
        echo_color "0;31" "Error encountered during: $last_command. See details below and in log file at $LOGFILE. Exiting..."
        cat "$TEMPFILE" >> "$LOGFILE"  # Append the captured error to the log
        cleanup
        exit 1
    fi
    > "$TEMPFILE"  # Clear the temp file for the next command
}

# Function to handle cleanup on exit
cleanup() {
    if [[ -n "$GIT_PID" ]]; then
        kill -9 "$GIT_PID" 2>/dev/null
    fi
    rm -f "$TEMPFILE"
    echo_color "0;31" "Script interrupted. Cleanup complete."
}

# Trap Ctrl+C (SIGINT) and call the cleanup function
trap 'echo_color "0;31" "Ctrl+C detected. Exiting..."; cleanup; exit 1' SIGINT

# Function to monitor the progress of the `git clone` command
monitor_git_clone() {
    local last_size=0
    local current_size=0
    local elapsed_time=0

    while kill -0 $1 2>/dev/null; do
        sleep "$GIT_CLONE_MONITOR_INTERVAL"
        current_size=$(du -sb "$INSTALL_DIR" 2>/dev/null | cut -f1)

        if [[ "$current_size" == "$last_size" ]]; then
            elapsed_time=$((elapsed_time + GIT_CLONE_MONITOR_INTERVAL))
            if [[ "$elapsed_time" -ge "$GIT_CLONE_TIMEOUT" ]]; then
                echo_color "0;31" "No progress detected in $GIT_CLONE_TIMEOUT seconds. Aborting git clone."
                kill -9 $1 2>/dev/null
                wait $1 2>/dev/null  # Wait for the process to actually terminate
                check_status
                echo_color "0;31" "Git clone failed due to lack of progress. Exiting script."
                cleanup
                exit 1
            fi
        else
            elapsed_time=0
        fi

        last_size="$current_size"
    done

    wait $1  # Capture the final exit status of the process
    check_status  # Ensure we stop the script if the process failed
}

# Function to check and install missing dependencies
check_and_install_dependencies() {
    local dependencies=("git" "build-essential" "libtool" "autotools-dev" "automake" "pkg-config" "bsdmainutils" "python3" "libssl-dev" "libevent-dev" "libboost-system-dev" "libboost-filesystem-dev" "libboost-test-dev" "libboost-thread-dev" "libfmt-dev" "libsqlite3-dev" "libminiupnpc-dev" "libzmq3-dev" "libqt5gui5" "libqt5core5a" "libqt5dbus5" "qttools5-dev" "qttools5-dev-tools" "libqrencode-dev")
    local missing_deps=()
    for dep in "${dependencies[@]}"; do
        if ! dpkg -s $dep >/dev/null 2>&1; then
            missing_deps+=("$dep")
        fi
    done
    if [ ${#missing_deps[@]} -gt 0 ]; then
        check_network_connectivity  # Ensure network is active before attempting to install dependencies
        echo_color "0;34" "Installing missing dependencies: ${missing_deps[*]}"
        sudo apt-get update 2>>"$TEMPFILE" || check_status
        sudo apt-get install -y "${missing_deps[@]}" 2>>"$TEMPFILE" || check_status
    else
        echo_color "0;32" "All dependencies are already installed."
    fi
}

# Function to check network connectivity
check_network_connectivity() {
    local retries=0
    while ! ping -c 1 google.com &> /dev/null; do
        retries=$((retries + 1))
        echo_color "0;31" "Network is down. Retrying in 30 seconds... (Attempt $retries of $MAX_NETWORK_RETRIES)"
        sleep 30
        if [ $retries -ge $MAX_NETWORK_RETRIES ]; then
            echo_color "0;31" "Network connection failed after $MAX_NETWORK_RETRIES attempts. Exiting..."
            cleanup
            exit 1
        fi
    done
    echo_color "0;32" "Network connection is active."
}

# Function to prompt user before deleting directory with confirmation and 10-second countdown
prompt_for_deletion() {
    echo_color "0;33" "Installation directory $INSTALL_DIR already exists."
    echo_color "0;33" "Press 'K' within 10 seconds to keep the existing folder and stop the script, or the folder will be deleted automatically..."

    for i in {10..1}; do
        echo -ne "\r\033[0;33mCountdown: $i seconds remaining... Press 'K' to keep the folder. \033[0m"
        read -t 1 -n 1 -r key
        if [[ $key =~ ^[Kk]$ ]]; then
            echo -e "\n\033[0;33mKeeping the existing folder $INSTALL_DIR Stopping the script to allow for backups or other actions.\033[0m"
            echo_color "0;33" "User chose to keep the existing folder. Script stopped."
            cleanup
            exit 0
        fi
    done

    echo -e "\n\033[0;31mAutomatically proceeding with deletion...\033[0m"
    rm -rf "$INSTALL_DIR"
    echo_color "0;31" "Installation directory $INSTALL_DIR deleted automatically after countdown."
}

echo_color "0;32" "Starting Bitrae Node installation..."

# Install dependencies
check_and_install_dependencies

# Ensure network connectivity before performing the git clone
check_network_connectivity

# Prompt user before deleting the existing installation directory
if [ -d "$INSTALL_DIR" ]; then
    prompt_for_deletion
fi

# Cloning Bitrae Node repository if the directory was deleted or doesn't exist
if [ ! -d "$INSTALL_DIR" ]; then
    echo_color "0;34" "Cloning Bitrae Node repository"
    git clone --progress "$REPO_URL" "$INSTALL_DIR" 2>&1 | grep -v '^ ' &
    GIT_PID=$!
    monitor_git_clone $GIT_PID
    check_status

    # Explicitly check if the directory was successfully created
    if [ ! -d "$INSTALL_DIR" ]; then
        echo_color "0;31" "Failed to clone the repository. The installation directory does not exist. Exiting."
        cleanup
        exit 1
    fi
fi

# Change directory to INSTALL_DIR
echo_color "0;34" "Changing directory to $INSTALL_DIR"
cd "$INSTALL_DIR" 2>>"$TEMPFILE"
check_status

# Install BerkeleyDB
echo_color "0;34" "Installing BerkeleyDB"
./contrib/install_db4.sh "$(pwd)" 2>>"$TEMPFILE"
check_status

# Running autogen.sh
echo_color "0;34" "Preparing build configuration"
./autogen.sh 2>>"$TEMPFILE"
check_status

# Setting environment variable for BerkeleyDB
echo_color "0;34" "Setting BerkeleyDB Environment"
export BDB_PREFIX="$INSTALL_DIR/db4"
check_status

# Checking environment variables and directory contents
echo_color "0;34" "Checking environment variables"
ls "$BDB_PREFIX/lib" 2>>"$TEMPFILE"
check_status

# Setting Make configuration
echo_color "0;34" "Setting Make configuration"
./configure BDB_LIBS="-L${BDB_PREFIX}/lib -ldb_cxx-4.8" BDB_CFLAGS="-I${BDB_PREFIX}/include" 2>>"$TEMPFILE"
check_status

# Compile the binary
echo_color "0;32" "Creating Bitrae binary"
make 2>>"$TEMPFILE"
check_status

# Prompt user for installing binaries
read -p "Do you want to install the binaries? (y/n): " install_binaries
if [[ "$install_binaries" =~ ^[Yy]$ ]]; then
    echo_color "0;34" "Installing binaries"
    sudo make install 2>>"$TEMPFILE"
    check_status
    
    # Final completion message    
    echo_color "0;32" "Bitrae Node install complete. Binaries installed. Run command \033[1;33m\"bitraed\"\033[0;32m to start the node or run \033[1;33m\"bitrae-qt\"\033[0;32m to open GUI wallet, use the \033[1;33m\"-testnet\"\033[0;32m option to run the node in TESTNET."
else
    echo_color "0;32" "Bitrae Node install complete. Run command \033[1;33m\"./bitraed\"\033[0;32m in $INSTALL_DIR/src to start the node or \033[1;33m\"./bitrae-qt\"\033[0;32m in $INSTALL_DIR/src/qt to open the GUI wallet, use the \033[1;33m\"-testnet\"\033[0;32m option to run the node in TESTNET."
fi

# Clear the trap to avoid inadvertent triggering
trap - SIGINT

# Clean up temporary file
rm -f "$TEMPFILE"

