#include <iostream>

extern "C" {
    int do_math(int);
}

struct Point{
	int x;
	int y;
};

int main() {
	Point p;
	p.x = 123456;
//	std::cout << "domath: " << do_math(1) << std::endl;
}
