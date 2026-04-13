FROM ubuntu:22.04
USER root
ENV QT_VERSION=5.15.2
ENV DEBIAN_FRONTEND=noninteractive
ENV DISPLAY=:99
ENV QMAKESPEC=linux-g++-64
ENV QT_PATH=/opt/Qt
ENV QT_DESKTOP=$QT_PATH/${QT_VERSION}/gcc_64
ENV PATH=/usr/lib/ccache:$QT_DESKTOP/bin:$PATH
ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Europe/Oslo
RUN apt-get update && apt-get upgrade -y && \
    apt-get install -y \
    curl \
    libsdl2-dev \
    gstreamer1.0-gl \
    qtbase5-dev \
    qtchooser \
    qt5-qmake \
    libxcb-xinerama0 \
    locales \
    speech-dispatcher \
    pulseaudio-utils \
    rhvoice \
    rhvoice-english \
    && rm -rf /var/lib/apt/lists/*
# Route speech-dispatcher through RHVoice's slt voice via the sd_generic
# module — Ubuntu 22.04 ships no sd_rhvoice, so we shell out to RHVoice-test
# and pipe into paplay. PULSE env is hardcoded because sd_generic's subprocess
# doesn't reliably inherit it. Voice is forced to slt (ignoring $VOICE) so QGC
# can't pick a male voice via its default voice-type selection.
RUN printf '%s\n' \
      'Name "rhvoice"' \
      'GenericExecuteSynth "/bin/sh -c '"'"'export PULSE_SERVER=unix:/run/user/1000/pulse/native XDG_RUNTIME_DIR=/run/user/1000; echo \"$DATA\" | RHVoice-test -p slt | paplay'"'"'"' \
      'GenericCmdDependency "RHVoice-test"' \
      'AddVoice "en" "FEMALE1" "slt"' \
      'AddVoice "en" "MALE1"   "slt"' \
      'DefaultVoice "slt"' \
      > /etc/speech-dispatcher/modules/rhvoice.conf && \
    echo 'AddModule "rhvoice" "sd_generic" "rhvoice.conf"' >> /etc/speech-dispatcher/speechd.conf && \
    sed -i 's|^#\?DefaultModule.*|DefaultModule rhvoice|' /etc/speech-dispatcher/speechd.conf
RUN groupadd -r qgc -g 1000 && useradd -m -u 1000 -r -g qgc qgc
RUN mkdir app
ARG APPIMAGE_PATH="./QGroundControl.AppImage"
COPY ${APPIMAGE_PATH} /app
RUN chmod 755 /app
WORKDIR /app
# Reconfigure locale
RUN locale-gen en_US.UTF-8 && dpkg-reconfigure locales
ENV LANG=en_US.UTF-8
ENV LANGUAGE=en_US:en
ENV LC_ALL=en_US.UTF-8
USER qgc
ENV APPIMAGE_EXTRACT_AND_RUN=1
CMD ["./QGroundControl.AppImage"]
