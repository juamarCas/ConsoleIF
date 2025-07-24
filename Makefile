build:
	cd build && cmake .. -DCMAKE_BUILD_TYPE=Release

build-debug:
	cd build && cmake .. -DCMAKE_BUILD_TYPE=Debug

clean:
	rm -rf build
	mkdir build