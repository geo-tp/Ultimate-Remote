CXX ?= g++
BUILD_DIR ?= build-linux
TARGET ?= ultimate-remote
IR_TX_PIN ?= 12

CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -O2
CPPFLAGS += -Isrc -include src/Platform/Compatibility/ArduinoCompat.h \
	-DULTIMATE_REMOTE_LINUX=1 \
	-DULTIMATE_REMOTE_IR_TX_PIN=$(IR_TX_PIN)
LDFLAGS ?=
LDLIBS ?=

SOURCES := $(shell find src -name '*.cpp' \
	! -path 'src/Platform/Arduino/*')

OBJECTS := $(patsubst src/%.cpp,$(BUILD_DIR)/obj/%.o,$(SOURCES))
DEPS := $(OBJECTS:.o=.d)

.PHONY: all clean deb

all: $(BUILD_DIR)/$(TARGET)

deb:
	python3 packaging/build-deb.py

$(BUILD_DIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(BUILD_DIR)/obj/%.o: src/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MMD -MP -c $< -o $@

-include $(DEPS)

clean:
	$(RM) -r $(BUILD_DIR)
