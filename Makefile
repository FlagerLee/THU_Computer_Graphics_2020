all:
	g++ main.cpp template/png/lodepng.cpp -o main -O3 -Xpreprocessor -fopenmp -lomp -std=c++17