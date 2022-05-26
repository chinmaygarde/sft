# This project uses CMake and Git sub-modules. This Makefile is just in place
# to make common tasks easier.

.PHONY: build/sft

run: build/sft
	./build/sft

build/sft: build/build.ninja
	ninja -C build

build/build.ninja:
	mkdir -p build
	cmake -G Ninja -B build

clean:
	rm -rf build

sync:
	git submodule update --init --recursive
