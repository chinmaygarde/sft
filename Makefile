# This project uses CMake and Git sub-modules. This Makefile is just in place
# to make common tasks easier.

.PHONY: clean build

run: build
	@./build/src/playground/playground

test: build
	ctest --test-dir build -R $(TEST_FILTER)

build: build/build.ninja
	@cmake --build build

build/build.ninja:
	@mkdir -p build
	cmake -G Ninja -B build -DCMAKE_BUILD_TYPE=Release

clean:
	@rm -rf build

sync:
	@git submodule update --init --recursive -j 8
