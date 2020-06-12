all:
	g++ main.cpp template/png/lodepng.cpp -o main -g -Xpreprocessor -fopenmp -lomp -std=c++17
