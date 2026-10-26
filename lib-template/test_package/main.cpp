// Built as a SEPARATE project that only knows the library by its package name.
// If this compiles, links and runs, the library is consumable the way it claims.
#include <mylib/mylib.hpp>
#include <cstdio>

int main() {
    auto ok = mylib::square(3) == 9;
    std::printf("test_package: %s\n", ok ? "ok" : "FAILED");
    return ok ? 0 : 1;
}
