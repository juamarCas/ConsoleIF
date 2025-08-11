build-release:
	cd build && cmake .. -DCMAKE_BUILD_TYPE=Release -DDEBUG_MODE=OFF

build-debug:
	cd build && cmake .. -DCMAKE_BUILD_TYPE=Debug -DDEBUG_MODE=ON

build-tests:
	cd build && cmake .. -DCMAKE_BUILD_TYPE=Debug -DDEBUG_MODE=ON -DBUILD_TESTS=ON

clean:
	rm -rf build
	mkdir build