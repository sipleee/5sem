eigen: main.cpp matrix_io.cpp qr_algorithm.cpp utils.cpp
	g++ -std=c++98 -O2 -o eigen main.cpp matrix_io.cpp qr_algorithm.cpp utils.cpp

clean:
	rm -f eigen
