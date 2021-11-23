# This Makefile ties together the steps required to install C++ and
# Python dependencies. It only works on Linux, but can serve as
# self-documenting instructions for building on Windows.
# As such, they are ordered roughly in chronological order.

# We use marker files '.done' to track whether the dependencies have
# changed since the last install.
all: compile_project

# Create Python virtual environment
venv/bin/activate:
	python3 -m venv venv

# Install Python requirements into the virtual environment
venv/.done: requirements.txt venv/bin/activate
	. venv/bin/activate
	pip install -r requirements.txt
	touch $@

# Install C++ dependencies
build/.deps_done: conanfile.txt venv/.done
	mkdir -p build
	. venv/bin/activate
	conan install -if build --build=missing .
	touch $@

# Set up CMake project
build/.cmake_done: build/.deps_done CMakeLists.txt
	cd build && cmake ..
	touch $@

# Delegate the compilation of the project to the CMake - generated Makefiles.
compile_project: build/.cmake_done
	make -C build

clean:
	rm -rf venv
	rm -rf build

.PHONY: clean compile_project
