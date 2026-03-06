CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2 -Iinclude -MMD -MP
LDFLAGS := -lpthread

GTEST_LIBS := $(shell pkg-config --libs gtest gtest_main 2>/dev/null)
ifeq ($(strip $(GTEST_LIBS)),)
	GTEST_LIBS := -lgtest -lgtest_main
endif

SRC_DIR := src
TEST_DIR := test
OBJ_DIR := obj
BIN_DIR := bin

SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
TEST_FILES := $(wildcard $(TEST_DIR)/*.cpp)

APP_OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/app/%.o,$(SRC_FILES))
APP_TARGET := $(BIN_DIR)/pucch_codes_modeling

TEST_OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/test/%.o,$(filter-out $(SRC_DIR)/main.cpp,$(SRC_FILES)))
TEST_OBJS += $(patsubst $(TEST_DIR)/%.cpp,$(OBJ_DIR)/test/%.o,$(TEST_FILES))
TEST_TARGET := $(BIN_DIR)/pucch_tests

APP_SYMLINK := pucch_codes_modeling
DEPS := $(APP_OBJS:.o=.d) $(TEST_OBJS:.o=.d)

.PHONY: all app test run_app run_tests clean rebuild

all: app test

app: $(APP_TARGET)
test: $(TEST_TARGET)

$(APP_TARGET): $(APP_OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)
	@chmod +x $@

$(TEST_TARGET): $(TEST_OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS) $(GTEST_LIBS)
	@chmod +x $@

$(OBJ_DIR)/app/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)/app
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/test/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)/test
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/test/%.o: $(TEST_DIR)/%.cpp | $(OBJ_DIR)/test
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR)/app $(OBJ_DIR)/test:
	mkdir -p $@

run_app: app
	./$(APP_TARGET) $(ARGS)

run_tests: test app
	@ln -sf $(APP_TARGET) $(APP_SYMLINK) 2>/dev/null || cp $(APP_TARGET) $(APP_SYMLINK)
	@chmod +x $(APP_SYMLINK)
	./$(TEST_TARGET)
	@rm -f $(APP_SYMLINK)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(APP_SYMLINK) *.gcno *.gcda *.json /tmp/test_*.json

rebuild: clean all

-include $(DEPS)
.PRECIOUS: $(APP_OBJS) $(TEST_OBJS)