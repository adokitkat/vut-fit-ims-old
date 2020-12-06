CXX=g++
CXXFLAGS=-std=c++17 -Wall -Wpedantic# -Werror
DEBUG=-g
RELEASE=-O3
LIBS=-lglut -lGLU -lGL

SRCDIR=src/
TMPDIR=tmp/
DOCDIR=doc/
BINDIR=

LOGIN=11_xmlkvy00_xmudry01
OUTPUT=ims

.PHONY: all build build-debug release debug run run-terminal build-run

all: build-debug

# Building the program
build: release
build-debug: debug

# Release building
release: $(SRCDIR)main.cpp
	$(CXX) $(CXXFLAGS) $(RELEASE) $(SRCDIR)main.cpp $(LIBS) -o $(BINDIR)$(OUTPUT)

debug: $(SRCDIR)main.cpp
	$(CXX) $(CXXFLAGS) $(DEBUG) $(SRCDIR)main.cpp $(LIBS) -o $(BINDIR)$(OUTPUT)

run:
	./ims -g

run-terminal:
	./ims

build-run: all run