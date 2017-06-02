#include <iostream>

extern "C" {
    int do_math(int*, int);
	int test();
}

int func(int arr[3]){
	return arr[2];
}

struct Point{
	int x;
	int y;
};

int main() {
	int a = 1, b = 2;
	int arr[3] = { 1,2,3 };
	int c = arr[2];
	do_math(arr, 1);
	// do_math(1);
	// std::cout << "domath: " << do_math(1) << std::endl;
}
// clang output.o testmain.cpp -o test
// clang -S -emit-llvm testmain.cpp 