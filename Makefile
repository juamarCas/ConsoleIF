LOG_DIR := build/log

.PHONY: all build-release build-debug build-tests clean

build-release:
	cd build && cmake .. -DCMAKE_BUILD_TYPE=Release -DDEBUG_MODE=OFF

build-debug:
	cd build && cmake .. -DCMAKE_BUILD_TYPE=Debug -DDEBUG_MODE=ON

build-tests: prepare
	@echo "Building tests..."
	( cd build && cmake .. -DCMAKE_BUILD_TYPE=Debug -DDEBUG_MODE=ON -DBUILD_TESTS=ON ) 2>&1 | tee $(LOG_DIR)/build_tests.log

tests: build-tests
	@echo "Running tests..."
	(cd build && make -j8) 2>&1 | tee $(LOG_DIR)/conf_tests.log
	(cd build && ./ConsoleIfTests) 2>&1 | tee $(LOG_DIR)/run_tests.log


prepare: clean
	@mkdir -p $(LOG_DIR)

clean:
	rm -rf build
	mkdir build