#include <stdio.h>
#include "day1.h"
#include "day1func.h"

int main() {
  
  printf("Running test suite...\n");

  int lockVal = 9;
  Direction dir = LEFT;
  int amount = 9;

  char dirStr[6];
  if (dir == LEFT) sprintf(dirStr, "left");
  else sprintf(dirStr, "right");
  printf("Turning lock %s by %d from %d\n", dirStr, amount, lockVal);
  int zeroes = turnLock(&lockVal, dir, amount);
  printf("Result: %d\n with %d zero hits", lockVal, zeroes);

  return 0;
}
