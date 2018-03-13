#!/bin/sh
cd build/wavesim || echo "Error: Please run this script from the project's root directory as ./scripts/valgrind-tests-linux.sh"

echo "Started valgrind..."
valgrind --num-callers=50 \
	--leak-resolution=high \
	--leak-check=full \
	--track-origins=yes \
	--time-stamp=yes \
	./wavesim_tests $1 2>&1 | tee ../../wavesim_tests.grind
cd .. && cd ..
