CXX = g++
CXXFLAGS = -std=c++20 -I../../implementations/cpp/include -Wall -Wextra -fPIC
LDFLAGS = -shared

BUILD_DIR = build
TARGET = $(BUILD_DIR)/chrono_id.so

# Handle macOS
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	TARGET = $(BUILD_DIR)/chrono_id.dylib
endif

SRC = src/chrono_id.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(SRC) -o $(TARGET)

test: all
	python3 tests/test_chrono_id_sqlite.py

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all test clean
