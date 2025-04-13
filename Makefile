CC = g++
CFLAGS = -std=c++14 -Wall -O2
LIBS = -lgmp
INCLUDES = -I./include

# Binary names
MAIN = main
PRNG_BENCHMARK = prng_benchmark
PRIMALITY_BENCHMARK = primality_benchmark

# Experiment binaries
MEASURE_PRNG_TIME = experiments/bin/measure_prng_time
MEASURE_PRIMALITY_TIME = experiments/bin/measure_primality_time
CONTINUOUS_OPERATION = experiments/bin/continuous_operation

# Source files
MAIN_SRC = src/main.cpp
PRNG_BENCHMARK_SRC = src/prng/prng_benchmark.cpp
PRIMALITY_BENCHMARK_SRC = src/primality/primality_benchmark.cpp

# Experiment source files
MEASURE_PRNG_TIME_SRC = src/experiments/measure_prng_time.cpp
MEASURE_PRIMALITY_TIME_SRC = src/experiments/measure_primality_time.cpp
CONTINUOUS_OPERATION_SRC = src/experiments/continuous_operation.cpp

# Object files
MAIN_OBJ = $(MAIN_SRC:.cpp=.o)
PRNG_BENCHMARK_OBJ = $(PRNG_BENCHMARK_SRC:.cpp=.o)
PRIMALITY_BENCHMARK_OBJ = $(PRIMALITY_BENCHMARK_SRC:.cpp=.o)

# Experiment object files
MEASURE_PRNG_TIME_OBJ = $(MEASURE_PRNG_TIME_SRC:.cpp=.o)
MEASURE_PRIMALITY_TIME_OBJ = $(MEASURE_PRIMALITY_TIME_SRC:.cpp=.o)
CONTINUOUS_OPERATION_OBJ = $(CONTINUOUS_OPERATION_SRC:.cpp=.o)

# All headers (for dependency tracking)
HEADERS = $(wildcard include/*/*.h)

# Default target
all: $(MAIN) $(PRNG_BENCHMARK) $(PRIMALITY_BENCHMARK)

# Build everything including experiment tools
everything: all experiments

# Experiments target
experiments: $(MEASURE_PRNG_TIME) $(MEASURE_PRIMALITY_TIME) $(CONTINUOUS_OPERATION)

# Create experiment directories
experiment-dirs:
	mkdir -p experiments/bin experiments/results/timing experiments/results/energy

# Main program
$(MAIN): $(MAIN_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# PRNG benchmark
$(PRNG_BENCHMARK): $(PRNG_BENCHMARK_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# Primality benchmark
$(PRIMALITY_BENCHMARK): $(PRIMALITY_BENCHMARK_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# Experiment tools
$(MEASURE_PRNG_TIME): $(MEASURE_PRNG_TIME_OBJ) | experiment-dirs
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

$(MEASURE_PRIMALITY_TIME): $(MEASURE_PRIMALITY_TIME_OBJ) | experiment-dirs
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

$(CONTINUOUS_OPERATION): $(CONTINUOUS_OPERATION_OBJ) | experiment-dirs
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# Compile source files
%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

# Run benchmarks
bench: all
	./$(PRNG_BENCHMARK)
	./$(PRIMALITY_BENCHMARK)

# Run benchmarks without safety checks (WARNING: may take a long time)
bench-unsafe: all
	./$(PRNG_BENCHMARK)
	./$(PRIMALITY_BENCHMARK) --no-safety

# Run benchmarks with only 2048-bit Miller-Rabin test
bench-2048: all
	./$(PRNG_BENCHMARK)
	./$(PRIMALITY_BENCHMARK) --only-2048

# Run benchmarks with both 2048-bit and 4096-bit Miller-Rabin tests
bench-4096: all
	./$(PRNG_BENCHMARK)
	./$(PRIMALITY_BENCHMARK) --include-4096

# Run tests (can be expanded with actual test cases)
test: all
	@echo "Testing primality of known primes..."
	./$(MAIN) test 101 --algorithm=mr
	./$(MAIN) test 101 --algorithm=bpsw
	@echo "Testing primality of known composite numbers..."
	./$(MAIN) test 100 --algorithm=mr
	./$(MAIN) test 100 --algorithm=bpsw
	@echo "Testing primality of Carmichael number..."
	./$(MAIN) test 561 --algorithm=mr
	./$(MAIN) test 561 --algorithm=bpsw

# Setup for RISC-V experiments
riscv-setup: everything
	chmod +x experiments/scripts/*.sh
	@echo "Experiment tools and scripts are ready for RISC-V measurements"

# Clean
clean:
	rm -f $(MAIN) $(PRNG_BENCHMARK) $(PRIMALITY_BENCHMARK)
	rm -f $(MAIN_OBJ) $(PRNG_BENCHMARK_OBJ) $(PRIMALITY_BENCHMARK_OBJ)
	rm -f $(MEASURE_PRNG_TIME) $(MEASURE_PRIMALITY_TIME) $(CONTINUOUS_OPERATION)
	rm -f $(MEASURE_PRNG_TIME_OBJ) $(MEASURE_PRIMALITY_TIME_OBJ) $(CONTINUOUS_OPERATION_OBJ)
	rm -f results/*.csv

# Clean experiments only
clean-experiments:
	rm -f $(MEASURE_PRNG_TIME) $(MEASURE_PRIMALITY_TIME) $(CONTINUOUS_OPERATION)
	rm -f $(MEASURE_PRNG_TIME_OBJ) $(MEASURE_PRIMALITY_TIME_OBJ) $(CONTINUOUS_OPERATION_OBJ)

# Install (for Linux/macOS)
install: all
	mkdir -p $(DESTDIR)/usr/local/bin
	cp $(MAIN) $(DESTDIR)/usr/local/bin/
	cp $(PRNG_BENCHMARK) $(DESTDIR)/usr/local/bin/
	cp $(PRIMALITY_BENCHMARK) $(DESTDIR)/usr/local/bin/

# Uninstall (for Linux/macOS)
uninstall:
	rm -f $(DESTDIR)/usr/local/bin/$(MAIN)
	rm -f $(DESTDIR)/usr/local/bin/$(PRNG_BENCHMARK)
	rm -f $(DESTDIR)/usr/local/bin/$(PRIMALITY_BENCHMARK)

# Phony targets
.PHONY: all everything experiments experiment-dirs bench bench-unsafe bench-2048 bench-4096 test riscv-setup clean clean-experiments install uninstall 