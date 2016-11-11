FROM debian:sid

RUN apt-get update
RUN apt-get -y install \
    build-essential \
    ca-certificates \
    gengetopt \
    g++-6 \
    libboost-dev \
    openjdk-8-jdk \
    openjdk-8-jre \
    openjdk-8-jre-headless \
    wget \
    xxd

RUN apt-get -y install \
    unzip

RUN wget -O bazel-installer.sh -q \
  https://github.com/bazelbuild/bazel/releases/download/0.4.0/bazel-0.4.0-installer-linux-x86_64.sh \
  && bash bazel-installer.sh \
  && rm bazel-installer.sh
