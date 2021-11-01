#include <stdio.h>

int foo() { printf("%d\n", 10); }

int fooo(int x, int y) { printf("%d\n", x + y); }

int foooo(int x, int y, int z) {
  printf("%d\n", z + y + z);
  return x + y + z;
}