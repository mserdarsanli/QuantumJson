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

# Catch Test Framework [v1.5.8]
RUN wget https://raw.githubusercontent.com/philsquared/Catch/e27c4ee04282f60aefcc9b1062a74f92cf6c1a2b/single_include/catch.hpp -O /usr/local/include/catch.hpp

# nlohmann/json [v2.0.7]
RUN wget https://github.com/nlohmann/json/releases/download/v2.0.7/json.hpp -O /usr/local/include/nlohmann_json.hpp


RUN apt-get -y install \
    cmake

# jsoncpp [v1.7.7]
RUN bash -c "                                                                        \
    (                                                                                \
        cd $(mktemp -d)                                                              \
        && wget https://github.com/open-source-parsers/jsoncpp/archive/1.7.7.zip     \
        && unzip 1.7.7.zip                                                           \
        && cd jsoncpp-1.7.7                                                          \
        && cmake .                                                                   \
        && make install                                                              \
    )                                                                                \
"
