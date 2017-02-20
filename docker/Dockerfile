# The MIT License (MIT)
#
# Copyright (c) 2016 Mustafa Serdar Sanli
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

FROM ubuntu:16.04

RUN apt-get update
RUN apt-get -y install \
    build-essential \
    ca-certificates \
    cmake \
    flex \
    gengetopt \
    groff \
    g++ \
    libboost-dev \
    libmpc-dev \
    openjdk-8-jdk \
    openjdk-8-jre \
    openjdk-8-jre-headless \
    python3 \
    ruby \
    ruby-dev \
    unzip \
    vim-common \
    wget

RUN apt-get -y install \
    zlib1g-dev \
    bash-completion

# Install ronn (manpage generator)
RUN gem install ronn

RUN wget -O bazel.deb -q \
  https://github.com/bazelbuild/bazel/releases/download/0.4.3/bazel_0.4.3-linux-x86_64.deb \
  && dpkg -i bazel.deb \
  && apt-get install -f \
  && rm bazel.deb

# Build & install GCC7
RUN  cd $(mktemp -d)                                                           \
  && wget  https://github.com/gcc-mirror/gcc/archive/master.zip                \
  && unzip master.zip                                                          \
  && cd gcc-master                                                             \
  && mkdir build                                                               \
  && cd build                                                                  \
  && ../configure --disable-multilib                                           \
  && make                                                                      \
  && make install

# Catch Test Framework [v1.5.8]
RUN wget https://raw.githubusercontent.com/philsquared/Catch/e27c4ee04282f60aefcc9b1062a74f92cf6c1a2b/single_include/catch.hpp -O /usr/local/include/catch.hpp

# nlohmann/json [v2.1.0]
RUN wget https://github.com/nlohmann/json/releases/download/v2.1.0/json.hpp -O /usr/local/include/nlohmann_json.hpp


# jsoncpp [v1.8.0]
RUN bash -c "                                                                        \
    (                                                                                \
           cd $(mktemp -d)                                                           \
        && wget https://github.com/open-source-parsers/jsoncpp/archive/1.8.0.zip     \
        && unzip 1.8.0.zip                                                           \
        && cd jsoncpp-1.8.0                                                          \
        && cmake .                                                                   \
        && make install                                                              \
    )                                                                                \
"

# rapidjson [v1.1.0]
RUN bash -c "                                                                        \
    (                                                                                \
           cd $(mktemp -d)                                                           \
        && wget https://github.com/miloyip/rapidjson/archive/v1.1.0.zip              \
        && unzip v1.1.0.zip                                                          \
        && cd rapidjson-1.1.0                                                        \
        && cmake .                                                                   \
        && make CXX_FLAGS='-Wimplicit-fallthrough' install                           \
    )                                                                                \
"

# staticjson [v0.5.0]
RUN bash -c "                                                                        \
    (                                                                                \
           cd $(mktemp -d)                                                           \
        && wget https://github.com/netheril96/StaticJSON/archive/0.5.0.zip           \
        && unzip 0.5.0.zip                                                           \
        && cd StaticJSON-0.5.0                                                       \
        && mv include/staticjson /usr/local/include                                  \
        && g++ -c src/staticjson.cpp                                                 \
        && ar rvs libstaticjson.a staticjson.o                                       \
        && mv libstaticjson.a /usr/local/lib                                         \
    )                                                                                \
"
