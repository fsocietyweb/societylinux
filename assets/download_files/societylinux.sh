#!/bin/bash

# --- SLR (Society Linux Repository) ---
# Expanded LFS-style repository with source archive mappings
declare -A REPO_URLS
REPO_URLS=(
    # Original core utilities
    ["neofetch"]="https://github.com/dylanaraps/neofetch/archive/refs/tags/7.1.0.tar.gz"
    ["htop"]="https://github.com/htop-dev/htop/archive/refs/tags/3.3.0.tar.gz"
    ["tree"]="https://gitlab.com/OldManProgrammer/unix-tree/-/archive/2.1.1/unix-tree-2.1.1.tar.gz"
    
    # Networking & Security
    ["nmap"]="https://github.com/nmap/nmap/archive/refs/tags/v7.95.tar.gz"
    ["curl"]="https://github.com/curl/curl/archive/refs/tags/curl-8_7_1.tar.gz"
    
    # Utilities & Text Editors
    ["tmux"]="https://github.com/tmux/tmux/releases/download/3.4/tmux-3.4.tar.gz"
    ["nano"]="https://www.nano-editor.org/dist/v8/nano-8.0.tar.gz"
    ["git"]="https://mirrors.edge.kernel.org/pub/software/scm/git/git-2.44.0.tar.gz"
)

# --- Color Palettes ---
GREEN='\033[0;32m'
BRIGHT_GREEN='\033[1;32m'
WHITE='\033[1;37m'
GRAY='\033[0;37m'
RED='\033[0;31m'
NC='\033[0m'

# --- 1. Root Check ---
if [ "$EUID" -ne 0 ]; then
    echo -e "${RED}Error: Society Linux operations require root privileges.${NC}"
    echo -e "Please run it as Sudo if you are not trusted please check the code"
    exit 1
fi

# --- 2. Input & Flags Parsing ---
# Checking if the first argument is a valid flag
FLAG="$1"
PACKAGE="$2"

if [[ "$FLAG" != "--syspack" && "$FLAG" != "--rootpack" && "$FLAG" != "--uninstall" ]]; then
    echo -e "${RED}Invalid Action Flag.${NC}"
    echo -e "Usage:"
    echo -e "  Install:   sudo install --syspack {package_name}"
    echo -e "  Uninstall: sudo install --uninstall {package_name}"
    exit 1
fi

if [[ -z "$PACKAGE" ]]; then
    echo -e "${RED}Error: No package name specified.${NC}"
    exit 1
fi

# Validate if the package exists in the SLR
SRC_URL="${REPO_URLS[$PACKAGE]}"
if [ -z "$SRC_URL" ]; then
    echo -e "${RED}Error: Package '$PACKAGE' does not exist in the SLR (Society Linux Repository).${NC}"
    exit 1
fi

# --- Header Display ---
echo -e "${GREEN}=================================================="
echo -e "${BRIGHT_GREEN}         SOCIETY LINUX CORE BUILD ENGINE          "
echo -e "${GRAY}             [ LFS Compilation Layer ]            "
echo -e "${GREEN}==================================================${NC}"

# --- 3. Step 1: Detect Distro/OS Environment ---
echo -e "${GRAY}[1/3] Identifying Host OS Environment...${NC}"
if [ -f /etc/os-release ]; then
    . /etc/os-release
    HOST_OS="$NAME"
else
    HOST_OS=$(uname -s)
fi
echo -e "      -> Host System Detected: ${WHITE}$HOST_OS${NC}"


# --- 4. EXECUTE UNINSTALLATION ---
if [[ "$FLAG" == "--uninstall" ]]; then
    echo -e "${GRAY}[2/3] Querying SLR database for target binaries...${NC}"
    echo -e "${GRAY}[3/3] Removing compiled configurations and tracking hooks...${NC}"
    echo -e "\n${WHITE}--- PURGING COMPILED BINARIES (LFS Method) ---${NC}"

    # Handle custom removal commands depending on the layout of standard tools
    # LFS packages compiled with --prefix=/usr/local place their binaries there
    SUCCESS=0

    if [ -f "/usr/local/bin/$PACKAGE" ]; then
        rm -f "/usr/local/bin/$PACKAGE"
        echo -e "${GRAY}Removed binary: /usr/local/bin/$PACKAGE${NC}"
        SUCCESS=1
    fi
    
    # Check share/man pages or standard install scripts destinations
    if [ -d "/usr/local/share/$PACKAGE" ]; then
        rm -rf "/usr/local/share/$PACKAGE"
        echo -e "${GRAY}Removed Shared resources: /usr/local/share/$PACKAGE${NC}"
        SUCCESS=1
    fi

    # Fallback to absolute manual cleaning path tracking
    if [ $SUCCESS -eq 1 ]; then
        echo -e "\n${BRIGHT_GREEN}Success: '$PACKAGE' successfully uninstalled from system root!${NC}"
    else
        echo -e "\n${RED}Warning: Clean uninstall complete, but no direct binaries were found in /usr/local/bin/.${NC}"
    fi
    exit 0
fi


# --- 5. EXECUTE INSTALLATION ---
echo -e "${GRAY}[2/3] Querying SLR (Society Linux Repository)...${NC}"
echo -e "      -> Target Found: ${WHITE}$PACKAGE Source Recipe Located.${NC}"

echo -e "${GRAY}[3/3] Beginning Native Compilation Flow...${NC}"

# Create build directory workspace
BUILD_DIR="/tmp/society_lfs_build_${PACKAGE}"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR" || exit 1

echo -e "      -> Downloading source tarball..."
curl -L -s "$SRC_URL" -o "${PACKAGE}.tar.gz"
if [ $? -ne 0 ]; then
    echo -e "${RED}Error: Failed to fetch source code from mirror.${NC}"
    exit 1
fi

echo -e "      -> Unpacking build tree..."
tar -xzf "${PACKAGE}.tar.gz" --strip-components=1 2>/dev/null
if [ $? -ne 0 ]; then
    # Some github releases don't use nested structures, fallback extract
    tar -xzf "${PACKAGE}.tar.gz" 2>/dev/null
fi

echo -e "\n${WHITE}--- COMPILING FROM SOURCE (LFS Method) ---${NC}"

# Standard compilation step sequences
if [ -f ./configure ]; then
    echo -e "${GRAY}Running ./configure script...${NC}"
    ./configure --prefix=/usr/local > /dev/null 2>&1
fi

echo -e "${GRAY}Executing machine compilation (make)...${NC}"
make > /dev/null 2>&1

echo -e "${GRAY}Installing compiled binaries to system root (make install)...${NC}"
make install > /dev/null 2>&1

if [ $? -ne 0 ] && [ ! -f "/usr/local/bin/$PACKAGE" ]; then
    echo -e "${RED}Error: Compilation failed or binaries could not map to system root.${NC}"
    exit 1
fi

# Cleanup build workspace
cd /tmp || exit
rm -rf "$BUILD_DIR"

echo -e "\n${BRIGHT_GREEN}Success: '$PACKAGE' successfully built and installed to system root!${NC}"