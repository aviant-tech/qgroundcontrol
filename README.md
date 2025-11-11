# QGroundControl Ground Control Station

[![Releases](https://img.shields.io/github/release/mavlink/QGroundControl.svg)](https://github.com/mavlink/QGroundControl/releases)
[![Travis Build Status](https://travis-ci.org/mavlink/qgroundcontrol.svg?branch=master)](https://travis-ci.org/mavlink/qgroundcontrol)
[![Appveyor Build Status](https://ci.appveyor.com/api/projects/status/crxcm4qayejuvh6c/branch/master?svg=true)](https://ci.appveyor.com/project/mavlink/qgroundcontrol)

[![Gitter](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/mavlink/qgroundcontrol?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

_QGroundControl_ (QGC) is an intuitive and powerful ground control station (GCS) for UAVs.

The primary goal of QGC is ease of use for both first time and professional users.
It provides full flight control and mission planning for any MAVLink enabled drone, and vehicle setup for both PX4 and ArduPilot powered UAVs. Instructions for _using QGroundControl_ are provided in the [User Manual](https://docs.qgroundcontrol.com/en/) (you may not need them because the UI is very intuitive!)

All the code is open-source, so you can contribute and evolve it as you want.
The [Developer Guide](https://dev.qgroundcontrol.com/en/) explains how to [build](https://dev.qgroundcontrol.com/en/getting_started/) and extend QGC.

Key Links:

- [Website](http://qgroundcontrol.com) (qgroundcontrol.com)
- [User Manual](https://docs.qgroundcontrol.com/en/)
- [Developer Guide](https://dev.qgroundcontrol.com/en/)
- [Discussion/Support](https://docs.qgroundcontrol.com/en/Support/Support.html)
- [Contributing](https://dev.qgroundcontrol.com/en/contribute/)
- [License](https://github.com/mavlink/qgroundcontrol/blob/master/COPYING.md)

### Running unit tests

[Guide](https://aviant.atlassian.net/wiki/spaces/TECHNICAL/pages/994803739/Running+unit+tests)

### Update the latest QGC version in AWS
You need to have an AWS account with the necessary permissions for ECRPublic and configure the [AWS CLI](https://docs.aws.amazon.com/cli/latest/userguide/cli-chap-configure.html).
1. Log in to AWS ecr public: `aws ecr-public get-login-password --region us-east-1 | docker login --username AWS --password-stdin public.ecr.aws`
2. Pull the desired release (image tag) you want to set to latest: `docker pull public.ecr.aws/m7l5v7s4/qgroundcontrol:<release version>`
3. Tag the release as "latest": `docker tag public.ecr.aws/m7l5v7s4/qgroundcontrol:<release version> public.ecr.aws/m7l5v7s4/qgroundcontrol:latest`
4. Push the "latest" tag: `docker push public.ecr.aws/m7l5v7s4/qgroundcontrol:latest`
5. Log out of AWS ecr public: `docker logout public.ecr.aws`

### Building AppImage using Docker
To creat an AppImage locally, run:
```docker buildx build -o . .``

The output directory chan be changed using the argument to the `-o` flag.

Object files are cached between runs, so only the linking has to be re-done on each build. (This gives sub-minute builds)


### Running QGroundControl locally with Docker

This guide shows how to build and run QGroundControl locally using Docker on Linux.

#### Prerequisites
- Docker installed on your system
- Git (to clone the repository)

#### Steps

1. **Clone the repository** (if you haven't already):
   ```bash
   git clone https://github.com/mavlink/qgroundcontrol.git
   cd qgroundcontrol
   ```

2. **Create a build directory** on your host system:
   ```bash
   # Create a directory where the build output will be stored
   mkdir -p ~/builds/qgroundcontrol/linux
   ```

3. **Build the Docker image**:
   ```bash
   docker build --file ./deploy/docker/Dockerfile-build-linux -t qgc-linux-docker .
   ```

4. **Run the build process**:
   ```bash
   # This will compile QGroundControl inside the Docker container
   # Replace ~/builds/qgroundcontrol/linux with your preferred build directory
   docker run --rm \
     -v ${PWD}:/project/source \
     -v ~/builds/qgroundcontrol/linux:/project/build \
     qgc-linux-docker
   ```

5. **Run QGroundControl**:
   ```bash
   # Execute the compiled application
   ~/builds/qgroundcontrol/linux/staging/QGroundControl
   ```

#### Automated script

You can also use this automated script to perform all steps at once. This is recommended when doing development, as you dont have to wait for each step to be finished before running the next one.

```bash
#!/bin/bash

# Function to check if a command was successful
check_success() {
  if [ $? -ne 0 ]; then
    echo "Error: $1 failed"
    exit 1
  fi
}

# Set build directory (modify this path as needed)
BUILD_DIR="$HOME/builds/qgroundcontrol/linux"

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
```

#### Notes
- The build process may take some time on the first run, but will be faster on subsequent runs due to docker cache 
- Make sure you have sufficient disk space for the build artifacts
- You can modify the `BUILD_DIR` variable in the script to use a different location for build outputs
- To access simulator drones in the local QGC-app, run the Multi-command simulator at the same time