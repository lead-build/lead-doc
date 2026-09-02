#include "myrustlib.h"
#include <stdio.h>

int main() {
    printf("C and Rust Example\n");
    printf("Calling Rust function: add_in_rust(10, 20) = %d\n", add_in_rust(10, 20));
    return 0;
}
