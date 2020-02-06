all:
	g++ main.cpp -o main -O3 -Xpreprocessor -fopenmp -lomp -std=c++17