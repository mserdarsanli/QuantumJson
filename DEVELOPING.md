# QuantumJson Development

Development on QuantumJson relies on
[Bazel build tool](https://github.com/bazelbuild/bazel),
[Catch test framework](https://github.com/philsquared/Catch),
[Ronn](https://github.com/rtomayko/ronn) and installation of other C++ Json
libraries (for benchmarks).

To keep the repository cleaner and not to bloat developer machines a Dockerfile is
provided. Therefore having `docker` is the only requisite for developing QuantumJson.

If you are only interested in using QuantumJson, you can simply build and install it using
the Makefile provided.

## Docker Usage

Build the image, this will install required libraries in docker

    docker build -t quantumjson-build-server ./docker

Boot into it (replace the absolute repo path with yours). This will mount code directory
to /QuantumJson in the docker instance, and will give you a shell.

    docker run -t -i -v /home/serdar/Repo/QuantumJson:/QuantumJson quantumjson-build-server /bin/bash

Inside docker use following commands:

    cd /QuantumJson
    
    # Run all tests
    bazel --batch test --genrule_strategy=standalone --spawn_strategy=standalone //tests:all //src:all
    
    # Run benchmarks
    bazel --batch build --genrule_strategy=standalone --spawn_strategy=standalone //benchmark:all

Or use following halper make targets

    make docker-build
    make docker-run-tests
    make docker-run-benchmarks
