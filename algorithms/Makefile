CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I. -I../graphs -g -fprofile-arcs -ftest-coverage
LDFLAGS = -lgcov

SRC = main.cpp \
      AlgorithmServer.cpp \
      AlgorithmFactory.cpp \
      MSTAlgorithm.cpp \
      HamiltonAlgorithm.cpp \
      SCCAlgorithm.cpp \
      MaxCliqueAlgorithm.cpp \
      ../graphs/Graph.cpp

OBJ = $(SRC:.cpp=.o)

TARGET = algorithm_server

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
	@echo ">>> Now run the server and client manually as described in the instructions."
	@echo ">>> After you exit the server, run: make gcov-report"

gcov-report:
	gcov -b -c $(SRC)

clean:
	rm -f $(OBJ) $(TARGET) *.gcda *.gcno *.gcov callgrind.out.* gmon.out coverage.info