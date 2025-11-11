# syntax=docker/dockerfile:1.6
FROM ubuntu:22.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive
ENV QT_VERSION=5.15.2
ENV SOURCE_DIR=/src
ENV BUILD_TYPE=DailyBuild

WORKDIR ${SOURCE_DIR}

# Base dependencies
RUN apt-get update && apt-get install -y --no-install-recommends \
    git wget curl python3 python3-pip build-essential ninja-build cmake \
    libgl1-mesa-dev libx11-dev libxext-dev libxi-dev libxrender-dev \
    libxcb1-dev libxcb-glx0-dev libsdl2-dev libfuse2 patchelf binutils \
    libgstreamer1.0-0 libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev \
    libfreetype6-dev libfontconfig1-dev ca-certificates rsync file \
    && rm -rf /var/lib/apt/lists/*

# Install Qt using aqt (mirrors install-qt-action@v3)
RUN pip install --no-cache-dir aqtinstall==3.2.0 && \
    python3 -m aqt install-qt linux desktop ${QT_VERSION} -O /opt/qt --modules qtcharts
ENV PATH="/opt/qt/${QT_VERSION}/gcc_64/bin:${PATH}"

# Copy source tree
COPY . .

# Build QGroundControl (using qmake, with cache for build artifacts)
RUN --mount=type=cache,target=/build_cache \ 
    mkdir -p /build_cache/qgc && cd /build_cache/qgc && \
    qmake -r ${SOURCE_DIR}/qgroundcontrol.pro CONFIG+=installer CONFIG+=${BUILD_TYPE} && \
    make -j$(nproc)

# Package AppImage
RUN --mount=type=cache,target=/build_cache \
    cd /build_cache/qgc && \
    mkdir -p staging package && \
    bash ${SOURCE_DIR}/deploy/create_linux_appimage.sh ${SOURCE_DIR} ./staging ./package && \
    cp -r ./package/QGroundControl.AppImage /tmp/

# Final stage: Export AppImage
FROM scratch AS export
COPY --from=builder /tmp/QGroundControl.AppImage /
