#include <iostream>

extern "C" {
    int do_math(int);
}

int main() {
	std::cout << "domath: " << do_math(1) << std::endl;
}
