CXXFLAGS = -std=c++20 -O0 -Iinclude -Wall -Wextra --coverage
LDFLAGS = --coverage
BUILD_DIR = build

.PHONY: all test clean coverage

all: test

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/test.o: tests/test.cpp include/chrono_id.hpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c tests/test.cpp -o $(BUILD_DIR)/test.o

$(BUILD_DIR)/test_bin: $(BUILD_DIR)/test.o
	$(CXX) $(CXXFLAGS) $(BUILD_DIR)/test.o -o $(BUILD_DIR)/test_bin $(LDFLAGS)

$(BUILD_DIR)/test_json.o: tests/test_json.cpp include/chrono_id.hpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c tests/test_json.cpp -o $(BUILD_DIR)/test_json.o

$(BUILD_DIR)/test_json_bin: $(BUILD_DIR)/test_json.o
	$(CXX) $(CXXFLAGS) $(BUILD_DIR)/test_json.o -o $(BUILD_DIR)/test_json_bin $(LDFLAGS)

test: $(BUILD_DIR)/test_bin test-json
	./$(BUILD_DIR)/test_bin

test-json: $(BUILD_DIR)/test_json_bin
	./$(BUILD_DIR)/test_json_bin

coverage: test
	gcov -abcfu -o $(BUILD_DIR) tests/test.cpp
	mv *.gcov $(BUILD_DIR)/

report: coverage
	@echo "--- Coverage Report ---"
	@grep -A 3 "File 'include/chrono_id.hpp'" $(BUILD_DIR)/*.gcov || true

clean:
	rm -rf $(BUILD_DIR)
