# This project uses CMake and Git sub-modules. This Makefile is just in place
# to make common tasks easier.

.PHONY: build/sft

run: build/sft
	./build/sft

build/sft: build/build.ninja
	ninja -C build

build/build.ninja:
	mkdir -p build
	cmake -G Ninja -B build -DCMAKE_BUILD_TYPE=Release

clean:
	rm -rf build

sync:
	git submodule update --init --recursive

shaders:
	mkdir -p build
	rm -f build/impeller.vert.spv
	rm -f build/impeller.frag.spv
	glslc -g -O -fauto-bind-uniforms -fauto-map-locations assets/impeller.vert -o build/impeller.vert.spv
	glslc -g -O -fauto-bind-uniforms -fauto-map-locations assets/impeller.frag -o build/impeller.frag.spv
	spirv-cross build/impeller.vert.spv --cpp --output src/shaders/impeller.vert.gen.h
	spirv-cross build/impeller.frag.spv --cpp --output src/shaders/impeller.frag.gen.h
	clang-format -i src/shaders/impeller.vert.gen.h
	clang-format -i src/shaders/impeller.frag.gen.h

