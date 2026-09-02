#include "mylib.h"
#include <stdio.h>

int main() {
    printf("Vendored Libraries Example\n");
    printf("Calling vendored library function: my_multiply(6, 7) = %d\n", my_multiply(6, 7));
    return 0;
}
