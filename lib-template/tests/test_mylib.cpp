#include <mylib/mylib.hpp>
#include "check.hpp"

// Compile-time tests: if these fail, the test does not even build.
static_assert(mylib::square(4) == 16);
static_assert(mylib::greeting("x") == "hello, x");

int main() {
    CHECK_EQ(mylib::square(-3), 9);
    CHECK_EQ(mylib::greeting("world"), "hello, world");
    CHECK(!mylib::greeting("").empty());
    return check::result();
}
