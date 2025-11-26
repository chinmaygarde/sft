run: build
	@./build/src/playground/playground

build: gen
	cmake --build --preset default

gen:
	cmake --preset default

clean:
	rm -rf build

sync:
	git submodule update --init --recursive -j 8
