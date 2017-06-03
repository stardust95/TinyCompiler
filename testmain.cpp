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
    int arr[3] = {44,55,66};
    // int c = arr[1];
	printf("%p\n", arr+1);
	do_math(arr,1);
	test();
	// int c = arr[2];
	// printf("|%p|%d|\n", &(arr[1]), arr[1]);
	// do_math(arr+1, 1);
	// int c = test();
	// printf("%d, %d, %d", arr[0], arr[1], c);
	// do_math(1);
	// std::cout << "domath: " << do_math(1) << std::endl;
}
// clang output.o testmain.cpp -o test
// clang -S -emit-llvm testmain.cpp 