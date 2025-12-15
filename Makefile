CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g -Iinclude

LDFLAGS = -pthread

EXECUTABLE = server
SRC_DIR = src
OBJ_DIR = obj

SOURCES = $(wildcard $(SRC_DIR)/*.cpp)

OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))


all: $(EXECUTABLE) load_test

$(EXECUTABLE): $(OBJECTS)
	@echo "LD   $@"
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "Build finalizado. Execute com: ./$(EXECUTABLE)"
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@echo "CXX  $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

clean:
	@echo "Cleaning project..."
	@rm -rf $(OBJ_DIR) $(EXECUTABLE) load_test client

load_test: load_test.cpp
	@echo "CXX  load_test.cpp"
	$(CXX) $(CXXFLAGS) load_test.cpp -o load_test $(LDFLAGS)
	@echo "Build finalizado. Execute com: ./load_test"

client: client.cpp
	@echo "CXX  client.cpp"
	$(CXX) $(CXXFLAGS) client.cpp -o client $(LDFLAGS)
	@echo "Build finalizado. Execute com: ./client"

.PHONY: all clean load_test client
