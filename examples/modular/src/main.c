#include "lib.h"
#include "myrustlib.h"
#include <stdio.h>

int main() {
  say_hello();
  printf("Rust added %d + %d = %d\n", 2, 3, add_in_rust(2, 3));
  return 0;
}