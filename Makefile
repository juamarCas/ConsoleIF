build-release:
	cd build && cmake .. -DCMAKE_BUILD_TYPE=Release -DDEBUG_MODE=OFF

build-debug:
	cd build && cmake .. -DCMAKE_BUILD_TYPE=Debug -DDEBUG_MODE=ON

clean:
	rm -rf build
	mkdir build