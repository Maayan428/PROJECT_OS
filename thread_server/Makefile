# cd thread_server

# make clean
# make CXXFLAGS="-g -Wall -pthread"
# valgrind --tool=helgrind ./thread_server

# telnet localhost 9090

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I. -I../graphs -I../algorithms -g -fprofile-arcs -ftest-coverage
LDFLAGS = -lgcov -lpthread

SRC = main.cpp \
      ThreadServer.cpp \
      ../algorithms/AlgorithmFactory.cpp \
      ../algorithms/MSTAlgorithm.cpp \
      ../algorithms/HamiltonAlgorithm.cpp \
      ../algorithms/SCCAlgorithm.cpp \
      ../algorithms/MaxCliqueAlgorithm.cpp \
      ../graphs/Graph.cpp

OBJ = $(SRC:.cpp=.o)

TARGET = thread_server

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
	@echo ">>> Run the threaded server and clients manually now."
	@echo ">>> Then run: make gcov-report"

gcov-report:
	gcov -b -c $(SRC)

clean:
	@echo "Cleaning up object and coverage files..."
	@rm -vf *.o *.gcda *.gcno *.gcov $(TARGET) callgrind.out.* gmon.out coverage.info