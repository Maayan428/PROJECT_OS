CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
SRC = main.cpp Server.cpp ../graphs/Graph.cpp ../graphs/EulerCircle.cpp
TARGET = server

# Build the server
all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

# Run the server
run: $(TARGET)
	./$(TARGET)

# Memcheck
valgrind: $(TARGET)
	valgrind --leak-check=full ./$(TARGET)

# Helgrind (thread errors)
helgrind: $(TARGET)
	valgrind --tool=helgrind ./$(TARGET)

# Callgraph (for kcachegrind)
callgraph: $(TARGET)
	valgrind --tool=callgrind ./$(TARGET)
	kcachegrind callgrind.out.*

# Coverage
coverage: clean
	$(CXX) $(CXXFLAGS) --coverage -g $(SRC) -o $(TARGET)_cov
	echo "6 10 42" | ./$(TARGET)_cov
	echo "-1 5 10" | ./$(TARGET)_cov
	echo "abc 5 10" | ./$(TARGET)_cov
	lcov --capture --directory . --output-file coverage.info
	genhtml coverage.info --output-directory coverage-report
	@echo "Open coverage-report/index.html in a browser"

# Cleanup
clean:
	rm -f $(TARGET) $(TARGET)_cov *.gcno *.gcda coverage.info callgrind.out.* gmon.out
	rm -rf coverage-report

.PHONY: all clean run valgrind helgrind callgraph coverage