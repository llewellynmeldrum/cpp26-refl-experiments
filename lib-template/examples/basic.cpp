// Examples double as documentation: write them from the caller's side first.
#include <mylib/mylib.hpp>
#include <cstdio>

int main() {
    auto msg = mylib::greeting("from mylib");
    std::printf("%s (square(7) = %d)\n", msg.c_str(), mylib::square(7));
}
