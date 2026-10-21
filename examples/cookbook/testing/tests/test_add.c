#include "utils.h"
#include <stdio.h>

int main() {
    if (add(5, 3) != 8) {
        printf("add(5, 3) != 8\n");
        return 1;
    }
    return 0;
}
