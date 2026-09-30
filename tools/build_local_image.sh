#!/bin/bash
# Builds the QGroundControl docker image locally, the same way CI builds the image pushed to AWS ECR:
# the AppImage is built with the top level Dockerfile, then packaged with .github/docker/Dockerfile.
#
# Usage: tools/build_local_image.sh [image[:tag]]    (default: qgroundcontrol:local)
# Builds a StableBuild like the released images, set BUILD_TYPE=DailyBuild to override.

set -euo pipefail

IMAGE="${1:-qgroundcontrol:local}"
BUILD_TYPE="${BUILD_TYPE:-StableBuild}"
SOURCE_DIR="$(cd "$(dirname "$0")/.." && pwd)"
APPIMAGE_DIR="$(mktemp -d)"
trap 'rm -rf "$APPIMAGE_DIR"' EXIT

echo "Building $BUILD_TYPE AppImage..."
DOCKER_BUILDKIT=1 docker build --target export --build-arg BUILD_TYPE="$BUILD_TYPE" --output "type=local,dest=$APPIMAGE_DIR" "$SOURCE_DIR"

echo "Building image $IMAGE..."
docker build --build-arg APPIMAGE_PATH=QGroundControl.AppImage -f "$SOURCE_DIR/.github/docker/Dockerfile" -t "$IMAGE" "$APPIMAGE_DIR"

echo "Built $IMAGE"
