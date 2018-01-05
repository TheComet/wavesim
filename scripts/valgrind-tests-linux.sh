#!/bin/sh
cd build/wavesim || echo "Error: Please run this script from the project's root directory as ./scripts/valgrind-tests-linux.sh"

echo "Started valgrind..."
valgrind --num-callers=50 \
	--leak-resolution=high \
	--leak-check=full \
	--track-origins=yes \
	--time-stamp=yes \
	./test_wavesim 2>&1 | tee ../../test_wavesim.grind
cd .. && cd ..
