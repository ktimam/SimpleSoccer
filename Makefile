# Thanks to Job Vranish (https://spin.atomicobject.com/2016/08/26/makefile-c-projects/)
TARGET_EXEC := matchbackend

BUILD_DIR := ./build
SRC_DIRS := ./src

#COPY_FILES = $(BUILD_DIR)/Params.ini

# Find all the C and C++ files we want to compile
# Note the single quotes around the * expressions. Make will incorrectly expand these otherwise.
SRCS := $(shell find . -type d \( -path ./.git -o -path ./3rdparty -o -path ./.vs -o -path ./x64 -o -path ./build -o -path ./Common/lua-5.0 -o -path ./Common/luabind \) -prune -o -name '*.cpp' -print)

# String substitution for every C/C++ file.
# As an example, hello.cpp turns into ./build/hello.cpp.o
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

# String substitution (suffix version without %).
# As an example, ./build/hello.cpp.o turns into ./build/hello.cpp.d
DEPS := $(OBJS:.o=.d)

# Every folder in ./src will need to be passed to GCC so that it can find header files
INC_DIRS := $(shell find . -type d \( -path ./.git -o -path ./3rdparty -o -path ./.vs -o -path ./x64 -o -path ./build -o -path ./Common/lua-5.0 -o -path ./Common/luabind \) -prune -o -type d -print)
# Add a prefix to INC_DIRS. So moduleA would become -ImoduleA. GCC understands this -I flag
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

# The -MMD and -MP flags together generate Makefiles for us!
# These files will have .d instead of .o as the output.
SOFTFLOATFLAGS := -msoft-float -m64 -lsoft-fp -L.
CPPFLAGS := $(INC_FLAGS) $(SOFTFLOATFLAGS) -MMD -MP  -D LINUX

# The final build step.
$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	tr -d '\15\32' < Params.ini > $(BUILD_DIR)/Params.ini
	$(CXX) $(OBJS) -lstdc++ -lm $(SOFTFLOATFLAGS) -o $@ $(LDFLAGS)

# Build step for C source
$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# Build step for C++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) -v
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

#copy ini files (not working)
$(BUILD_DIR)/%.ini: %.ini
	@echo "Copying file"
	tr -d '\15\32' < $< > $@

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)

run:
	@./$(BUILD_DIR)/$(TARGET_EXEC)

# Include the .d makefiles. The - at the front suppresses the errors of missing
# Makefiles. Initially, all the .d files will be missing, and we don't want those
# errors to show up.
-include $(DEPS)