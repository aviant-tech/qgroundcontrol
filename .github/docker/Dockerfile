FROM ubuntu:focal
USER root
ENV QT_VERSION=5.15.2
ENV DEBIAN_FRONTEND noninteractive
ENV DISPLAY :99
ENV QMAKESPEC linux-g++-64
ENV QT_PATH /opt/Qt
ENV QT_DESKTOP $QT_PATH/${QT_VERSION}/gcc_64
ENV PATH /usr/lib/ccache:$QT_DESKTOP/bin:$PATH
ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Europe/Oslo
RUN apt-get update && apt-get upgrade -y
RUN apt-get install curl libsdl2-dev gstreamer1.0-gl qt5-default libxcb-xinerama0 -y
RUN groupadd -r qgc -g 1000 && useradd -m -u 1000 -r -g qgc qgc
RUN mkdir app
ARG APPIMAGE_PATH="./QGroundControl.AppImage"
COPY ${APPIMAGE_PATH} /app
RUN chmod 755 /app
WORKDIR /app
USER qgc
ENV APPIMAGE_EXTRACT_AND_RUN=1
CMD ["./QGroundControl.AppImage"]
