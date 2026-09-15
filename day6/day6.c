#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <assert.h>

#define LINE_LEN 4000
#define PRINT_MATH_PT1 false
#define PRINT_MATH_PT2 false

typedef unsigned long long uint64;

uint64 solveProblemsPt1(const int numRows, const int numCols, const short table[numRows][numCols]);
uint64 solveProblemsPt2(const int numRows, const int numCols, const char table[numRows][numCols]);
int numsPerLine(const char* str);
int findLen(const char* str);


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

  // get table dimensions
  char line[LINE_LEN];
  fgets(line, LINE_LEN, file);
  const int numCols = numsPerLine(line);
  const int rowLenPt2 = findLen(line);
  int numRows = 1;
  while (fgets(line, LINE_LEN, file) != NULL) {
    numRows++;
  }
  fseek(file, 0, SEEK_SET);
  printf("Pt 1 Dimensions: %d rows, %d cols\n", numRows, numCols);
  printf("Pt 2 Dimensions: %d rows, %d cols\n", numRows, rowLenPt2);

  // read numbers into table1, raw chars into table2
  short table1[numRows][numCols];
  char table2[numRows][rowLenPt2];
  int c, r = 0;
  while (strtol(fgets(line, LINE_LEN, file), NULL, 10)) { 
    strncpy(&table2[r][0], line, rowLenPt2); // cleans \n and \0
    char *endPtr = line;
    c = 0;
    int num = 1;
    while ((num = strtol(endPtr, &endPtr, 10)) != 0) {
      assert(c < numCols);
      table1[r][c] = num;
      c++;
    }
    r++;
  }

  // read arithmetic symbols
  strncpy(&table2[numRows-1][0], line, rowLenPt2);
  c = 0;
  char* sym = strtok(line, " ");
  while (sym != NULL) {
    table1[r][c] = *sym;
    c++;
    sym = strtok(NULL, " ");
  }
  
  uint64 pt1Sum = solveProblemsPt1(numRows, numCols, table1);
  uint64 pt2Sum = solveProblemsPt2(numRows, rowLenPt2, table2);

  fclose(file);

  printf("PART 1 ANSWER: %llu\n", pt1Sum); 
  printf("PART 2 ANSWER: %llu\n", pt2Sum); 

  return 0;
}

uint64 solveProblemsPt1(const int numRows, const int numCols, const short table[numRows][numCols]) {
  if (PRINT_MATH_PT2) printf("\nPart 1:\n");
  uint64 aggSum = 0;
  for (int c = 0; c < numCols; c++) {
    const char op = table[numRows-1][c]; // save operator type (mult or add)
    uint64 sumOrProduct = op == '*' ? 1 : 0;
    for (int r = 0; r < numRows-1; r++) {
      if (PRINT_MATH_PT1) {
        printf("%d ", table[r][c]);
        if (r != numRows-2) printf("%c ", op);
      }
      if (op == '*') sumOrProduct *= table[r][c];
      else if (op == '+') sumOrProduct += table[r][c];
    }
    if (PRINT_MATH_PT1) printf("= %llu\n", sumOrProduct);
    aggSum += sumOrProduct;
  }
  return aggSum;
}


uint64 solveProblemsPt2(const int numRows, const int numCols, const char table[numRows][numCols]) {
  if (PRINT_MATH_PT2) printf("\nPart 2:\n");
  uint64 aggSum = 0;
  uint64 sum = 0;
  uint64 product = 1;
  // right to left
  for (int c = numCols-1; c >= 0; c--) {
    int colVal = 0;
    // read column number
    for (int r = 0; r < numRows-1; r++) {
      if (48 <= table[r][c] && table[r][c] <= 57) {
        colVal *= 10;
        colVal += table[r][c] - '0';
      }
    }
    if (colVal == 0) continue; // skip blank columns
    product *= colVal;
    sum += colVal;
    if (PRINT_MATH_PT2) {
      printf("%d ", colVal);
    }

    // check if column contains operator (thus terminating this problem)
    const char op = table[numRows-1][c];
    if (op != ' ') {
      uint64 *result;
      if (op == '*') {
        aggSum += product;
        result = &product;
      }
      else if (op == '+') {
        aggSum += sum;
        result = &sum;
      }
      if (PRINT_MATH_PT2) printf("= %llu (op = %c)\n", *result, op);
      c--; // skip following blank column
      sum = 0;
      product = 1;
    } else {
      if (PRINT_MATH_PT2) printf("op ");
    }
  }
  return aggSum;
}

// this assumes the input given does not contain the value 0
int numsPerLine(const char* str) {
  char *endPtr;
  strtol(str, &endPtr, 10);
  int i = 1;
  while (strtol(endPtr, &endPtr, 10)) {
    i++;
  }
  return i;
}

int findLen(const char* str) {
  int i = 0;
  while (str[i] != '\n') {
    i++;
  }
  return i;
}
