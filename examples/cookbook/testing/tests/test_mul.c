#include "utils.h"
#include <stdio.h>

int main() {
    if (mul(5, 3) != 15) {
        printf("mul(5, 3) != 15\n");
        return 1;
    }
    return 0;
}
