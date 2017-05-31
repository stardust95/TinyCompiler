#include <iostream>

extern "C" {
    int64_t do_math(int64_t);
}

int main() {
    std::cout << "domath: " << do_math(1) << std::endl;
}
