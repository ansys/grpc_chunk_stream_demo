# This Makefile ties together the steps required to install C++ and
# Python dependencies. It only works on Linux, but can serve as
# self-documenting instructions for building on Windows.
# As such, they are ordered roughly in chronological order.

# Commands with a preceding '@' are for the purposes of make only,
# there is no need to reproduce then on Windows.

PROTOC=build/protoc
PROTOS_DIR=src/protos
GRPC_CPP_PLUGIN_PATH=build/grpc_cpp_plugin
VENV_ACTIVATE=venv/bin/activate

SHELL := /bin/bash

all: compile_project src/protos/send_array.proto results/sync.csv

venv/bin/activate:
	@echo "### Creating Python virtual environment ###"
	python3 -m venv venv

venv/.done: requirements.txt venv/bin/activate
	@echo "### Installing Python dependencies ###"
	. $(VENV_ACTIVATE) && \
	pip install -U pip && \
	pip install -r requirements.txt && \
	conan profile new --detect --force grpc_cpp_benchmark && \
	conan profile update settings.compiler.libcxx=libstdc++11 grpc_cpp_benchmark && \
	conan remote add -f conancenter https://center.conan.io
	@touch $@

src/protos/send_array.proto src/common/types_lookup.hpp src/client/sync_client.cpp: % : render_templates.py venv/.done %.in
	@echo "### Rendering .proto file from template ###"
	. $(VENV_ACTIVATE) && python $< $@.in $@

build/.deps_done: conanfile.txt venv/.done
	@echo "### Installing C++ dependencies ###"
	mkdir -p build
	. $(VENV_ACTIVATE) && conan install -if build --build=missing --profile=grpc_cpp_benchmark .
	@touch $@

generated_src/.generate_done: src/protos/send_array.proto build/.deps_done
	@echo "### Compiling .proto file to C++ source ###"
	$(PROTOC) --proto_path=$(PROTOS_DIR) --cpp_out=generated_src --grpc_out=generated_src --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) send_array.proto
	@touch $@

build/.cmake_done: build/.deps_done CMakeLists.txt **/CMakeLists.txt generated_src/.generate_done
	@echo "### Setting up CMake ###"
	cd build && cmake ..
	@touch $@

compile_project: build/.cmake_done build/.deps_done generated_src/.generate_done src/common/types_lookup.hpp src/client/sync_client.cpp
	@echo "### Compiling the C++ source ###"
	$(MAKE) -C build

results/sync.csv: runner/run_sync.py compile_project
	@echo "### Running synchronous tests ###"
	. $(VENV_ACTIVATE) && python $<

clean:
	rm -rf venv
	rm -rf build
	rm -rf generated_src/*.cc
	rm -rf generated_src/*.h
	rm -rf **/.*done
	rm -f src/protos/*.proto
	rm -f src/types_lookup.hpp

.PHONY: clean compile_project all
