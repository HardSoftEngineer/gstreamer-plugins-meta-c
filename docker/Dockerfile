FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive
ENV LANG=C.UTF-8
ENV LC_ALL=C.UTF-8

RUN apt-get update && apt-get install -y \
    build-essential \
    pkg-config \
    cmake \
    git \
    ca-certificates \
    gstreamer1.0-tools \
    libgstreamer1.0-dev \
    libgstreamer-plugins-base1.0-dev \
    gstreamer1.0-plugins-base \
    gstreamer1.0-plugins-good \
    gstreamer1.0-plugins-bad \
    gstreamer1.0-plugins-ugly \
    gstreamer1.0-libav \
    python3 \
    python3-pip \
    jq \
    coreutils \
    procps \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /work

CMD ["tail", "-f", "/dev/null"]
