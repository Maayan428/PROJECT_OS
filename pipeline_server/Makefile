CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread -I. -I../graphs -I../algorithms -g -fprofile-arcs -ftest-coverage
LDFLAGS = -lgcov -lpthread

SRC = main.cpp \
      PipelineServer.cpp \
      PipelineStage.cpp \
      ../algorithms/AlgorithmFactory.cpp \
      ../algorithms/MSTAlgorithm.cpp \
      ../algorithms/HamiltonAlgorithm.cpp \
      ../algorithms/SCCAlgorithm.cpp \
      ../algorithms/MaxCliqueAlgorithm.cpp \
      ../graphs/Graph.cpp

OBJ = $(SRC:.cpp=.o)

TARGET = pipeline_server

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

valgrind-memcheck: $(TARGET)
	valgrind --leak-check=full --track-origins=yes ./$(TARGET)

valgrind-helgrind: $(TARGET)
	valgrind --tool=helgrind ./$(TARGET)

valgrind-callgrind: $(TARGET)
	valgrind --tool=callgrind ./$(TARGET)

coverage: clean $(TARGET)
	@echo ">>> Run the pipeline server and clients manually now."
	@echo ">>> Then run: make gcov-report"

gcov-report:
	gcov -b -c $(SRC)

clean:
	@echo "Cleaning up object and coverage files..."
	@find .. -type f \( -name '*.o' -o -name '*.gcda' -o -name '*.gcno' -o -name '*.gcov' \) -exec rm -v {} \;
	@rm -vf $(TARGET) callgrind.out.* gmon.out coverage.info