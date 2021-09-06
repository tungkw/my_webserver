SRC_DIR=src
BUILD_DIR=build
BIN_DIR=bin
FLAGS = -l pthread -std=c++14 -Wall -g -W -O

src_files = $(wildcard $(SRC_DIR)/*.cpp)

$(BIN_DIR)/server: $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(src_files))
	g++ $^ -o $(BIN_DIR)/server $(FLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	g++ -c $^ -I inc/ -o $@ $(FLAGS)

.PHONY: clean
clean:
	rm $(BIN_DIR)/server $(BUILD_DIR)/*