#include <iostream>

extern "C" {
    int do_math(int);
}

struct Point{
	int x;
	int y;
};

int main() {
	int a = 1, b = 2, c = 3;
	int arr[3] = { a,b };
	printf("%d", 1);
	// std::cout << "domath: " << do_math(1) << std::endl;
}
