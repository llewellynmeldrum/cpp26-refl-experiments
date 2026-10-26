# ===================================================================
# CMake Wrapper Makefile (library edition)
# ===================================================================
LIB         := mylib
SHELL       := /bin/bash
.SHELLFLAGS := -o pipefail -c
export CLICOLOR_FORCE := 1
RELPATH     := 2>&1 | sed "s|$(CURDIR)/||g"

# Toolchain: a file in cmake/toolchains/. Put `export TC=gcc-16` in your shell rc
# to make it your default, or pass it per command: make test TC=llvm
TC          ?= native
# Where `make install` puts things. ~/.local needs no sudo.
PREFIX      ?= $(HOME)/.local
# Which examples/<EX>.cpp `make run` runs.
EX          ?= basic

TOOLCHAIN    = $(CURDIR)/cmake/toolchains/$(TC).cmake
BUILD_DIR    = build/$(TC)-$*
STAGE       := $(CURDIR)/build/stage-$(TC)

# make -B / make -S, or B=1 / S=N
MAKE_LETTERS := $(firstword -$(MAKEFLAGS))
B ?= $(if $(findstring B,$(MAKE_LETTERS)),1)
S ?= $(if $(findstring S,$(MAKE_LETTERS)),2)
TOGGLES := $(if $(B),-D_TIME_REPORT=ON) $(if $(S),-D_MAX_ERRORS=$(S))

asan_ENV  := ASAN_OPTIONS=detect_leaks=0:color=always:abort_on_error=1:halt_on_error=1
ausan_ENV := $(asan_ENV) UBSAN_OPTIONS=color=always:print_stacktrace=1:halt_on_error=1

# specifies the default target instead of just picking the first one
.DEFAULT_GOAL := test

# -------------------------------------------------------------------
# Per-preset pattern targets: build-dev, test-asan, run-fast, ...
# -------------------------------------------------------------------
configure-%:
	@cmake --preset $* -B $(BUILD_DIR) --toolchain $(TOOLCHAIN) --no-warn-unused-cli $(TOGGLES) > /dev/null
	@ln -sf $(BUILD_DIR)/compile_commands.json compile_commands.json

build-%: configure-%
	@cmake --build $(BUILD_DIR) $(RELPATH)

test-%: build-%
	@$($*_ENV) ctest --test-dir $(BUILD_DIR) --output-on-failure $(RELPATH)

run-%: build-%
	@$($*_ENV) ./$(BUILD_DIR)/examples/$(EX)

build: build-dev
test:  test-dev
run:   run-dev
debug: build-dev
	lldb -o run -- build/$(TC)-dev/examples/$(EX)

# -------------------------------------------------------------------
# Install: release build, copied into $(PREFIX)
# -------------------------------------------------------------------
install: build-release
	cmake --install build/$(TC)-release --prefix $(PREFIX)
	@cp build/$(TC)-release/install_manifest.txt build/last_install_manifest.txt
	@echo "Installed $(LIB) to $(PREFIX). Consumers: find_package($(LIB)) with CMAKE_PREFIX_PATH=$(PREFIX)"

# CMake has no uninstall; it does record every file it installed.
uninstall:
	@test -f build/last_install_manifest.txt || { echo "No install manifest in build/. Nothing to do."; exit 1; }
	@xargs rm -fv < build/last_install_manifest.txt
	@rm build/last_install_manifest.txt

# -------------------------------------------------------------------
# Consumer tests: build test_package/ as a separate project that uses the library
# -------------------------------------------------------------------
# Install into a throwaway prefix under build/, then find_package() it from there.
package-test: build-release
	@rm -rf $(STAGE)
	@cmake --install build/$(TC)-release --prefix $(STAGE) > /dev/null
	@cmake -S test_package -B build/$(TC)-package-test -G Ninja --toolchain $(TOOLCHAIN) \
		-DCONSUME=find_package -DCMAKE_PREFIX_PATH=$(STAGE) > /dev/null
	@cmake --build build/$(TC)-package-test $(RELPATH)
	@./build/$(TC)-package-test/consumer

# Pull the library in with FetchContent, redirected at this checkout. No install, no network.
fetch-test:
	@cmake -S test_package -B build/$(TC)-fetch-test -G Ninja --toolchain $(TOOLCHAIN) \
		-DCONSUME=fetchcontent -DFETCHCONTENT_SOURCE_DIR_MYLIB=$(CURDIR) > /dev/null
	@cmake --build build/$(TC)-fetch-test $(RELPATH)
	@./build/$(TC)-fetch-test/consumer

# Everything worth running before tagging a version.
check: test-dev test-ausan package-test fetch-test
	@echo "check: all green for TC=$(TC)"

clean:
	cmake --build build/$(TC)-dev --target clean
clean-all:
	rm -rf build compile_commands.json
.PHONY: build test run debug install uninstall package-test fetch-test check clean clean-all help ? h

?: help
h: help
help:
	@for i in $$(seq $$(tput cols)); do printf "▂"; done
	@tput sgr0; tput bold;
	@echo "USAGE:"
	@tput sgr0;
	@echo "    make [flags] <target> [TC=native|gcc-16|llvm] [PREFIX=dir] [EX=example]"
	@for i in $$(seq $$(tput cols)); do printf "▂"; done
	@tput sgr0; tput bold; tput sitm;
	@echo "FLAGS:"
	@tput sgr0;
	@echo "    -B (B)enchmark compile time   -> Shows the time taken to compile each file."
	@echo "    -S (S)ingle error             -> fail outright immediately on the first error."
	@for i in $$(seq $$(tput cols)); do printf "▂"; done;
	@tput sgr0; tput bold; tput sitm;
	@echo "TARGETS:"
	@tput sgr0;
	@echo "    test (default)   build + run tests with the dev preset"
	@echo "    test-<preset>    same, other preset: dev cfast fast asan ausan"
	@echo "    build[-<preset>] build library, tests, examples"
	@echo "    run[-<preset>]   run examples/\$$EX (default: basic)"
	@echo "    install          release build, installed to \$$PREFIX (default: ~/.local)"
	@echo "    uninstall        delete the files the last install wrote"
	@echo "    package-test     install to build/stage-\$$TC, consume via find_package"
	@echo "    fetch-test       consume via FetchContent pointed at this checkout"
	@echo "    check            test-dev + test-ausan + package-test + fetch-test"
	@echo "    clean / clean-all"
	@for i in $$(seq $$(tput cols)); do printf "▂"; done;
	@tput sgr0;
