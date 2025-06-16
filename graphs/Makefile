CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
SRC = main.cpp Graph.cpp EulerCircle.cpp
TARGET = prog

# Default build
all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

# === Code Coverage ===
coverage: clean
	$(CXX) $(CXXFLAGS) --coverage -g $(SRC) -o prog_cov
	# Multiple runs to maximize coverage
	./prog_cov -v 6 -e 9 -s 1      # Odd degree node (no Euler)
	./prog_cov -v 6 -e 10 -s 42    # Even degrees (maybe Euler)
	./prog_cov -v 10 -e 20 -s 7    # More complex graph
	./prog_cov -v 4 -e 2 -s 99     # Disconnected or sparse
	./prog_cov -v 1 -e 0 -s 3      # Trivial graph
	lcov --capture --directory . --output-file coverage.info
	genhtml coverage.info --output-directory coverage-report
	@echo "Coverage report available in coverage-report/index.html"
	
# === Gprof Profiling ===
gprof: clean
	$(CXX) $(CXXFLAGS) -pg -O2 $(SRC) -o prog_gprof
	./prog_gprof -v 50 -e 100 -s 42
	gprof ./prog_gprof gmon.out > gprof_report.txt
	@echo "Gprof report written to gprof_report.txt"

# === Valgrind Memcheck ===
valgrind: $(TARGET)
	valgrind --leak-check=full ./$(TARGET) -v 10 -e 20 -s 1

# === Valgrind Callgraph ===
callgraph: $(TARGET)
	valgrind --tool=callgrind ./$(TARGET) -v 10 -e 20 -s 1
	kcachegrind callgrind.out.*

# === Cleanup ===
clean:
	rm -f $(TARGET) prog_cov prog_gprof gmon.out *.gcno *.gcda coverage.info
	rm -rf coverage-report callgrind.out.* gprof_report.txt

.PHONY: all clean coverage gprof valgrind callgraph