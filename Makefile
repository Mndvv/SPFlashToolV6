# Usage 
# make HOST=(linux/windows) BUILD_TYPE=(debug/release)
# The objects and execution file will be move to ../_Output/

export

HOST	            := $(if $(HOST),$(HOST),linux)
BUILD_TYPE          := $(if $(BUILD_TYPE),$(BUILD_TYPE),debug)

$(if $(filter debug release,$(BUILD_TYPE)),,$(error BUILD_TYPE "$(BUILD_TYPE)" invalid; should be debug or release))
$(if $(filter linux windows,$(HOST)),,$(error HOST "$(HOST)" invalid; should be linux or windows))

BASE_OUTPUT_PATH    := ../_Output
# tools
ifeq ($(HOST),linux)
OUTPUT_PATH         := $(BASE_OUTPUT_PATH)/$(HOST)/$(BUILD_TYPE)
TEE                 := tee
MAKE                := make
else
WORKING_DIR         := $(shell cd)
TOOLS_DIR           := $(WORKING_DIR)\Build\Tools\Windows
OUTPUT_PATH         := $(BASE_OUTPUT_PATH)\$(HOST)
TEE                 := $(TOOLS_DIR)\tee.exe
MAKE                := make.exe
endif

PLATFORM_MAKEFILE   := Build/build-$(HOST).mk

LOG := build.log

$(MAKECMDGOALS): recursive-make
recursive-make:
	$(MAKE) -f $(PLATFORM_MAKEFILE) BUILD_TYPE=$(BUILD_TYPE) HOST=$(HOST) $(MAKECMDGOALS) 2>&1 | $(TEE) $(LOG)