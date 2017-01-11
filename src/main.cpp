#include <cstdlib>

#include "EternityII.hpp"

int main(int, char * [])
{
    EternityII test("V4.txt", "Palette.txt");
    test.start(10*1000);
    return EXIT_SUCCESS;
}
