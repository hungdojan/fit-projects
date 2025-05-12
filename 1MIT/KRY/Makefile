# CPP project: Project2
TARGET_NAME=kry
BUILD_DIR=./Build
ZIP_NAME=230814

DEBUG_DIR=$(BUILD_DIR)/Debug
DEBUG_BIN_DIR=$(DEBUG_DIR)/bin
DEBUG_EXEC=$(DEBUG_BIN_DIR)/$(TARGET_NAME)

RELEASE_DIR=$(BUILD_DIR)/Release
RELEASE_BIN_DIR=$(RELEASE_DIR)/bin
RELEASE_EXEC=$(RELEASE_BIN_DIR)/$(TARGET_NAME)

##############################################

.PHONY: build build_release run run_release doc pack clean update_commands

build: $(DEBUG_EXEC)

run: $(DEBUG_EXEC)
	@$<

build_release: $(RELEASE_EXEC)

run_release: $(RELEASE_EXEC)
	@$<

doc: Doxygen
	doxygen

pack: src/* README.md Makefile CMakeLists.txt Doxygen
	rm -f *.zip
	zip -r $(ZIP_NAME) $^

clean:
	rm -rf doc *.zip $(BUILD_DIR)

##############################################

$(DEBUG_EXEC): $(DEBUG_DIR)/Makefile
	make -C$(DEBUG_DIR) -j8

$(DEBUG_DIR)/Makefile: CMakeLists.txt ./src/*
	cmake -H. -B$(DEBUG_DIR) -DCMAKE_BUILD_TYPE=Debug
	if [[ ! -f ./compile_commands.json ]]; then ln -s $(DEBUG_DIR)/compile_commands.json .; fi

$(RELEASE_EXEC): $(RELEASE_DIR)/Makefile
	make -C$(RELEASE_DIR) -j8

$(RELEASE_DIR)/Makefile: CMakeLists.txt ./src/*
	cmake -H. -B$(RELEASE_DIR) -DCMAKE_BUILD_TYPE=Release
	if [[ ! -f ./compile_commands.json ]]; then ln -s $(RELEASE_DIR)/compile_commands.json .; fi
