#include <stdlib.h>
#include "day1.h"

// updates lock value
// returns number of times zero was encountered
int turnLock(int *lockAddr, Direction direction, int val) {
  int zeroes = 0;
  int lockVal = *lockAddr;
  int startVal = lockVal;
  if (direction == LEFT) lockVal -= val;
  else lockVal += val;
  
  if (lockVal > 99) { // overflow
    zeroes += lockVal / 100; // count 0 occurences
    lockVal %= 100;
  }
  else if (lockVal < 0) { // underflow
    // count zeroes
    zeroes += abs(lockVal / 100); 
    if (startVal != 0) zeroes++; // weird edge case
    // bring lock within range
    lockVal %= 100;
    if (lockVal != 0) lockVal += 100; // tricky edge case
  }
  else { // normal
    if (lockVal == 0) zeroes++;
  }
  *lockAddr = lockVal; // final value
  
  return zeroes;
}
