SRC_DIR=src
BUILD_DIR=build
BIN_DIR=bin

src_files = $(wildcard $(SRC_DIR)/*.cpp)

$(BIN_DIR)/server: $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(src_files))
	g++ $^ -o $(BIN_DIR)/server

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	g++ -c $^ -I inc/ -o $@

.PHONY: clean
clean:
	rm $(BIN_DIR)/server $(BUILD_DIR)/*