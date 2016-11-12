
# It is preferable to use bazel files while development,
# Makefile's wil be provided for each release.

HDRS = src/Attributes.hpp \
       src/CodeGenerator.hpp \
       src/JsonParserLibrary.hpp \
       src/Parser.hpp \
       src/Tokenizer.hpp \
       src/CommandLineFlags.hpp \
       lib/Common.hpp \
       lib/LibCommonDef.hpp

OBJS = src/Attributes.o \
       src/CodeGenerator.o \
       src/JsonParserLibrary.o \
       src/Parser.o \
       src/Tokenizer.o \
       src/CommandLineFlags.o \
       src/Main.o

.PHONY: all clean install

all: jc

TMPFILE := $(shell mktemp)
lib/LibCommonDef.hpp: lib/Common.hpp
	echo -n "#include <array>\n\n"                >> $(TMPFILE)
	echo -n "std::array<unsigned char,\n"         >> $(TMPFILE)
	echo -n "           "                         >> $(TMPFILE)
	stat -c "%s" lib/Common.hpp                   >> $(TMPFILE)
	echo -n "           > libCommonData = {\n"    >> $(TMPFILE)
	xxd -i < lib/Common.hpp                       >> $(TMPFILE)
	echo -n "};\n"                                >> $(TMPFILE)
	mv -f $(TMPFILE) lib/LibCommonDef.hpp

CC = g++ -std=c++1y -iquote .

src/CommandLineFlags.hpp src/CommandLineFlags.cpp: src/CommandLineFlags.ggo
	gengetopt --input="src/CommandLineFlags.ggo" \
	    --c-extension=cpp --header-extension=hpp \
	    --file-name="CommandLineFlags"  --output-dir="src"

src/Attributes.o: src/Attributes.cpp $(HDRS)
	$(CC) -c src/Attributes.cpp -o src/Attributes.o
src/CodeGenerator.o: src/CodeGenerator.cpp $(HDRS)
	$(CC) -c src/CodeGenerator.cpp -o src/CodeGenerator.o
src/JsonParserLibrary.o: src/JsonParserLibrary.cpp $(HDRS)
	$(CC) -c src/JsonParserLibrary.cpp -o src/JsonParserLibrary.o
src/Parser.o: src/Parser.cpp $(HDRS)
	$(CC) -c src/Parser.cpp -o src/Parser.o
src/Tokenizer.o: src/Tokenizer.cpp $(HDRS)
	$(CC) -c src/Tokenizer.cpp -o src/Tokenizer.o
src/Main.o: src/Main.cpp $(HDRS)
	$(CC) -c src/Main.cpp -o src/Main.o
src/CommandLineFlags.o: src/CommandLineFlags.cpp $(HDRS)
	$(CC) -c src/CommandLineFlags.cpp -o src/CommandLineFlags.o

jc: $(OBJS) $(HDRS)
	$(CC) $(OBJS) -o jc

clean:
	rm -f jc
	rm -f $(OBJS)
	rm -f lib/LibCommonDef.hpp
	rm -f src/CommandLineFlags.hpp src/CommandLineFlags.cpp

install: jc
	cp -f jc /usr/local/bin/jc


# Docker specific helper commands
# Beware that using these commans will trigger rebuild every time, since docker
# instance will be gone after the command completes.

.PHONY: docker-build docker-run-tests docker-run-benchmarks

# Get installation directory (http://stackoverflow.com/a/23324703/620438)
ROOT_DIR:=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

docker-build:
	docker build --tag quantumjson-build-server ./docker

docker-run-tests:
	docker run --tty --interactive --volume "$(ROOT_DIR):/QuantumJson" \
	    --workdir /QuantumJson quantumjson-build-server \
	    bazel --batch test \
	        --genrule_strategy=standalone --spawn_strategy=standalone \
	        //tests:all //src:all

docker-run-benchmarks:
	docker run --tty --interactive --volume "$(ROOT_DIR):/QuantumJson" \
	    --workdir /QuantumJson quantumjson-build-server \
	    bazel --batch build \
	        --genrule_strategy=standalone --spawn_strategy=standalone \
	        //benchmark:all
