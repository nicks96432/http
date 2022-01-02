TARGET       := libhttp.a
TEST_TARGETS := test

SRC_DIR      := ./src
BUILD_DIR    := ./build
INC_DIR      := ./include
TEST_DIR     := ./test

SRCS         := $(shell find $(SRC_DIR) -name '*.cpp')
OBJS         := $(SRCS:%=$(BUILD_DIR)/%.o)

TEST_SRCS    := $(TEST_TARGETS:%=$(TEST_DIR)/%.cpp)
TEST_OBJS    := $(TEST_SRCS:%=$(BUILD_DIR)/%.o)

INC_FLAGS    := $(addprefix -I, $(INC_DIR))
CPPFLAGS     := $(INC_FLAGS)
CXXFLAGS     := -std=c++17
LDFLAGS      := -lmagic

all: SRCS := $(filter-out ./src/utils/Log.cpp, $(SRCS))
all: OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
all: CXXFLAGS += -O3 -DNDEBUG
all: $(BUILD_DIR)/$(TARGET)

debug: CXXFLAGS += -O0 -ggdb -Wall -fsanitize=undefined -fsanitize=address
debug: LDFLAGS += -fsanitize=undefined -fsanitize=address
debug: $(BUILD_DIR)/$(TARGET)
debug: $(BUILD_DIR)/test/$(TEST_TARGETS)

$(BUILD_DIR)/$(TARGET): $(OBJS)
	$(AR) -rcs $@ $(OBJS)

$(BUILD_DIR)/%.cpp.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) -c $< -o $@

$(BUILD_DIR)/test/$(TEST_TARGETS): $(TEST_OBJS) $(BUILD_DIR)/$(TARGET)
	$(CXX) $(LDFLAGS) $? -o $@

$(BUILD_DIR)/test/%.cpp.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	@rm -r $(BUILD_DIR)
