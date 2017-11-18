CXX = g++
COMPILE_FLAGS = -Wall -Wunused -Wshadow -Wstrict-aliasing -pedantic -Werror -std=c++11 -O2 -c -MMD
LINK_FLAGS =


BIN_DIR = bin
OBJ_DIR = obj
SRC_DIR = UnifiedStreamingTask


.PHONY: all clean dirs


all: dirs ts_splitter ts_splitter_tests


dirs:
	mkdir -p $(OBJ_DIR)/test && mkdir -p $(BIN_DIR)


SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(subst $(SRC_DIR), $(OBJ_DIR), $(SOURCES:.cpp=.o))
-include $(OBJECTS:.o=.d)


SOURCES_TEST = $(wildcard $(SRC_DIR)/test/*.cpp) $(SRC_DIR)/error.cpp $(SRC_DIR)/output_name_generator.cpp $(SRC_DIR)/output_writer.cpp $(SRC_DIR)/payload_parser.cpp $(SRC_DIR)/program_options.cpp $(SRC_DIR)/ts_reader.cpp
OBJECTS_TEST = $(subst $(SRC_DIR), $(OBJ_DIR), $(SOURCES_TEST:.cpp=.o))
-include $(OBJECTS_TEST:.o=.d)


ts_splitter: dirs $(OBJECTS)
	$(CXX) $(LINK_FLAGS) $(filter-out $<, $^) -o $(BIN_DIR)/$@


ts_splitter_tests: dirs $(OBJECTS_TEST)
	$(CXX) $(LINK_FLAGS) $(filter-out $<, $^) -o $(BIN_DIR)/$@


$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(COMPILE_FLAGS) $< -o $@


$(OBJ_DIR)/%.o: $(SRC_DIR)/test/%.cpp
	$(CXX) $(COMPILE_FLAGS) $< -o $@


clean:
	@rm -rf  $(BIN_DIR) & rm -rf $(OBJ_DIR)

