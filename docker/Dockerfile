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
           cd $(mktemp -d)                                                           \
        && wget https://github.com/open-source-parsers/jsoncpp/archive/1.7.7.zip     \
        && unzip 1.7.7.zip                                                           \
        && cd jsoncpp-1.7.7                                                          \
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
        && make install                                                              \
    )                                                                                \
"

# staticjson [v0.4.1]
RUN bash -c "                                                                        \
    (                                                                                \
           cd $(mktemp -d)                                                           \
        && wget https://github.com/netheril96/StaticJSON/archive/0.4.1.zip           \
        && unzip 0.4.1.zip                                                           \
        && cd StaticJSON-0.4.1                                                       \
        && mv include/staticjson /usr/local/include                                  \
        && g++ -c src/staticjson.cpp                                                 \
        && ar rvs libstaticjson.a staticjson.o                                       \
        && mv libstaticjson.a /usr/local/lib                                         \
    )                                                                                \
"

# Install ronn (manpage generator)
RUN apt-get -y install \
    ruby \
    ruby-dev \
    groff
RUN gem install ronn
