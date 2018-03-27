# Must follow the format in the Naming section of https://vcvrack.com/manual/PluginDevelopmentTutorial.html
SLUG = squinkylabs-plug1

# Must follow the format in the Versioning section of https://vcvrack.com/manual/PluginDevelopmentTutorial.html
VERSION = 0.6.0 

# FLAGS will be passed to both the C and C++ compiler
FLAGS += -I./dsp/generators -I./dsp/utils -I./dsp/filters
FLAGS += -I./dsp/third-party/falco
FLAGS += -I./composites
FLAGS += -I./midi/controller
FLAGS += -I./midi/view
FLAGS += -I./midi/model
FLAGS += -I./util

CFLAGS +=
CXXFLAGS +=

# Command line variable to turn on "experimental" modules
ifdef _EXP
	FLAGS += -D _EXP
endif

# Careful about linking to shared libraries, since you can't assume much about the user's environment and library search path.
# Static libraries are fine.
LDFLAGS +=

# Add .cpp and .c files to the build
SOURCES += $(wildcard src/*.cpp)
SOURCES += $(wildcard dsp/**/*.cpp)
SOURCES += $(wildcard midi/**/*.cpp)
SOURCES += $(wildcard dsp/third-party/falco/*.cpp)

# Add files to the ZIP package when running `make dist`
# The compiled plugin is automatically added.
DISTRIBUTABLES += $(wildcard LICENSE*) res

# If RACK_DIR is not defined when calling the Makefile, default to two levels above
RACK_DIR ?= ../..

# Include the VCV Rack plugin Makefile framework
include $(RACK_DIR)/plugin.mk

include test.mk


    