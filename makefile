# Makefile
BP ?= btfn
CONFIG_ROOT := $(PIN_ROOT)/source/tools/Config
include $(CONFIG_ROOT)/makefile.config
TEST_TOOL_ROOTS := bp_${BP}
include $(TOOLS_ROOT)/Config/makefile.default.rules
