#include <stdio.h>
#include <stdlib.h> 
#include "day1.h"
#include "day1func.h"


int main (int argc, char **argv) {
  if (argc != 2) {
    printf("Usage: out input_file_name");
    exit(0);
  }

  // file input
  FILE* file = fopen(argv[1], "r");
  if (file == NULL) {
    fprintf(stderr, "could not read file :/");
    exit(1);
  }

  char line[10];
  int lockVal = 50;
  int zeroesPt1 = 0;
  int zeroesPt2 = 0;
  Direction direction;
  // for each line of input
  while (fgets(line, 10, file) != NULL) {
    direction = (line[0] == 'L') ? LEFT : RIGHT; // read direction
    int num = 0;
    int i = 1;
    // read number
    while (line[i] != '\n') {
      num *= 10; // make room for next digit
      num += line[i] - 48; // ascii things
      i++;
    }
    // update lock value and record zeroes
    zeroesPt2 += turnLock(&lockVal, direction, num);
    if (lockVal == 0) zeroesPt1++;
  }
  fclose(file);

  printf("PART 1 ANSWER: %d\n", zeroesPt1); 
  printf("PART 2 ANSWER: %d\n", zeroesPt2); 

  return 0;
}

