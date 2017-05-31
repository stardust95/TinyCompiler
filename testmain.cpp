#include <iostream>

extern "C" {
    int do_math(int);
}

struct Point{
	int x;
	int y;
};

int main() {
	int a[9];
	a[7] = a[8];	
//	std::cout << "domath: " << do_math(1) << std::endl;
}
