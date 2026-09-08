#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

int solvePt1(char *bank);
unsigned long long solvePt2(char* bank, int bankLen);
unsigned long long generalSolve(char *bank, int bankLen, int n);
int charToDigit(char c);
int findBankLen(char *bank);


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

  int pt1Joltage = 0;
  unsigned long long pt2Joltage = 0;
  unsigned long long general = 0;
  char bank[128];

  fgets(bank, 128, file);
  int bankLen = findBankLen(bank);
  fseek(file, 0, SEEK_SET);

  while (fgets(bank, 128, file) != NULL) {
    // pt1Joltage += solvePt1(bank);
    // pt2Joltage += solvePt2(bank, bankLen);
    pt1Joltage += generalSolve(bank, bankLen, 2);
    pt2Joltage += generalSolve(bank, bankLen, 12);
  }
  fclose(file);

  printf("PART 1 ANSWER: %d\n", pt1Joltage); 
  printf("PART 2 ANSWER: %llu\n", pt2Joltage); 
  printf("GENERAL ANSWER: %llu\n", general); 

  return 0;
}

// 2 digits
int solvePt1(char *bank) {
  int tens = charToDigit(bank[0]);
  int ones = charToDigit(bank[1]);
  int i = 1;
  while (bank[i] != '\n') {
    int currDigit = charToDigit(bank[i]); 
    if (currDigit > tens && bank[i+1] != '\n') {
      tens = currDigit;
      ones = charToDigit(bank[i+1]);
    } else if (currDigit > ones) {
      ones = currDigit;
    }
    i++;
  }
  return tens*10+ones;
}

// 12 digits
unsigned long long solvePt2(char* bank, int bankLen) {
  int saved[12] = {0};
  int i = 0; // indexes whole bank
  int end = -1; // track end of valid digits
  while (bank[i] != '\n') {
    int currDigit = charToDigit(bank[i]); 
    bool flag = false; // track whether a digit was already updated
    // 12-j -> digits left to fill
    // bankLen-i -> digits until end of bank
    // sequentially update most significant digit possible
    // once a digit is updated, all digits after it become invalid
    // if the loop finishes with no update, simply append the next digit to the end
    for (int j = 0; j <= end; j++) { // indexes saved number
      if (currDigit > saved[j] && 12-j <= bankLen-i) {
        saved[j] = currDigit;
        end = j;
        flag = true;
        break;
      }
    }
    if (!flag && end < 11) { // do not add if saved is full
      saved[++end] = currDigit;
    }
    i++;
  }

  unsigned long long joltage = 0;
  for (int i = 0; i < 12; i++) {
    joltage *= 10;
    joltage += saved[i];
  }
  // printf("Bank: %s", bank);
  // printf("%llu\n\n", joltage);
  return joltage;
}

// solve bank for n digit output 
unsigned long long generalSolve(char *bank, int bankLen, int n) {
  int saved[n];
  for (int idx = 0; idx < n; idx++) {
    saved[idx] = 0;
  }
  int i = 0; 
  int end = -1;
  while (bank[i] != '\n') {
    int currDigit = charToDigit(bank[i]); 
    bool flag = false;
    for (int j = 0; j <= end; j++) {
      if (currDigit > saved[j] && n-j <= bankLen-i) {
        saved[j] = currDigit;
        end = j;
        flag = true;
        break;
      }
    }
    if (!flag && end < n-1) { 
      saved[++end] = currDigit;
    }
    i++;
  }

  unsigned long long joltage = 0;
  for (int i = 0; i < n; i++) {
    joltage *= 10;
    joltage += saved[i];
  }
  return joltage;
}

int charToDigit(char c) {
  return c-'0';
}

int findBankLen(char *bank) {
  int i = 0;
  while (bank[i] != '\n') {
    i++;
  }
  return i;
}
