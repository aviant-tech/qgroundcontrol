#!/bin/bash

set -euxo pipefail

# Function to check if a command was successful
check_success() {
  if [ $? -ne 0 ]; then
    echo "Error: $1 failed"
    exit 1
  fi
}

# Set build directory (modify this path as needed)
if [ $# -eq 0 ]; then
  echo "Usage: $0 <build-directory>"
  echo "Error: No build directory specified."
  exit 1
fi
BUILD_DIR="$1"

# Create build directory if it doesn't exist
mkdir -p "$BUILD_DIR"
check_success "Creating build directory"

# Build Docker image
echo "Building Docker image..."
docker build --file ./deploy/docker/Dockerfile-build-linux -t qgc-linux-docker .
check_success "Docker build"

# Run the build process
echo "Building QGroundControl..."
docker run --rm -v ${PWD}:/project/source -v "$BUILD_DIR":/project/build qgc-linux-docker
check_success "QGroundControl build"

# Run QGroundControl
echo "Starting QGroundControl..."
"$BUILD_DIR/staging/QGroundControl"
check_success "QGroundControl execution"

echo "QGroundControl started successfully!"

