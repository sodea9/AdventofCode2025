#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <assert.h>

#define PRINT_GRID false
#define PRINT_END false
#define LINE_LEN 143

typedef struct {
  int r;
  int c;
} Coord;
typedef unsigned long long uint64;

void printGrid(const int numRows, const int numCols, const char grid[numRows][numCols]);
int findLen(const char* str);
void findS(char *line, Coord* s);
int countTachyonBeams(const int numRows, const int numCols, const char grid[numRows][numCols], Coord start);
int countSplits(const int numRows, const int numCols, char grid[numRows][numCols], Coord start);
uint64 countTimelinesRecursive(const int numRows, const int numCols, char grid[numRows][numCols], Coord p);
uint64 countTimelines(const int numRows, const int numCols, char grid[numRows][numCols], Coord start);


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

  // get schematic dimensions
  char line[LINE_LEN];
  fgets(line, LINE_LEN, file);
  Coord s;
  findS(line, &s); // we'll use this in a second :)
  const int numCols = findLen(line);
  int numRows = 1;
  while (fgets(line, LINE_LEN, file) != NULL) {
    numRows++;
  }
  fseek(file, 0, SEEK_SET);
  printf("Schematic Dimensions: %d rows, %d cols\n", numRows, numCols);

  char schematic[numRows][numCols];
  int r = 0;
  // read chars into 2d array
  while (fgets(line, LINE_LEN, file)) { 
    strncpy(&schematic[r][0], line, numCols); // cleans \n and \0
    r++;
  }
  assert(schematic[s.r][s.c] == 'S'); // sanity check
  fclose(file);

  int pt1Answer = countSplits(numRows, numCols, schematic, s);
  uint64 pt2Answer = countTimelines(numRows, numCols, schematic, s);
  // countTachyonBeams(numRows, numCols, schematic, s);

  if (PRINT_END) {
    printGrid(numRows, numCols, schematic);
    printf("\n");
  }

  printf("PART 1 ANSWER: %d splits\n", pt1Answer); 
  printf("PART 2 ANSWER: %llu timelines\n", pt2Answer); 

  return 0;
}

void printGrid(const int numRows, const int numCols, const char grid[numRows][numCols]) {
  for (int i = 0; i < numRows; i++) {
    for (int j = 0; j < numCols; j++) {
      printf("%c", grid[i][j]);
    }
    printf("\n");
  }
}

int findLen(const char* str) {
  int i = 0;
  while (str[i] != '\n') {
    i++;
  }
  return i;
}

// assumes S is in first line of schematic
// places output coord in given struct
void findS(char *line, Coord* s) {
  int i = 0;
  while (line[i] != 'S') {
    i++;
  }
  s->r = 0;
  s->c = i;
}

// counts beams accounting for merges
// top-down iterative solution
// assumes no splitters are on the edge of the grid
int countTachyonBeams(const int numRows, const int numCols, const char grid[numRows][numCols], Coord start) {
  bool beams[numCols]; // basically acts as a set
  memset(beams, 0, numCols*sizeof(bool)); // initialize to false
  beams[start.c] = true;
  for (int row = 1; row < numRows; row+=2) {
    for (int col = 1; col < numCols-1; col++) {
      // split
      if (grid[row][col] == '^') {
        beams[col] = false;
        beams[col-1] = true;
        beams[col+1] = true;
      }
    }
  }
  // count beams
  int count = 0;
  for (int i = 0; i < numCols; i++) {
    if (beams[i]) count++;
  }
  return count;
}


int countSplits(const int numRows, const int numCols, char grid[numRows][numCols], Coord start) {
  bool beams[numCols]; // basically acts as a set (one beam per column)
  memset(beams, 0, numCols*sizeof(bool)); // initialize to false
  beams[start.c] = true;
  int splits = 0;
  for (int row = 1; row < numRows; row++) {
    for (int col = 1; col < numCols-1; col++) {
      // split if beam present
      if (grid[row][col] == '^' && beams[col]) {
        splits++;
        beams[col] = false;
        beams[col-1] = true;
        beams[col+1] = true;
      }
    }
    if (PRINT_GRID || PRINT_END) {
      for (int i = 0; i < numCols; i++) {
        if (beams[i]) grid[row][i] = '|';
      }
    }
    if (PRINT_GRID) {
      printGrid(numRows, numCols, grid);
      printf("\n");
    }
  }
  return splits;
}

// top-down recursive divide-and-conquer solution (not tail-recursive though :( )
// assumes no splitters are on the edge of the grid
uint64 countTimelinesRecursive(const int numRows, const int numCols, char grid[numRows][numCols], Coord p) {
  while (grid[p.r][p.c] != '^') {
    if (++p.r == numRows-1) return 1; // reached bottom
  }
  Coord left, right;
  left.r = p.r; left.c = p.c-1;
  right.r = p.r; right.c = p.c+1;
  return countTimelines(numRows, numCols, grid, left) + countTimelines(numRows, numCols, grid, right);
}

// counts quantum beams ignoring merges
// top-down iterative solution that's way faster
// assuems no splitters occupy the edge
uint64 countTimelines(const int numRows, const int numCols, char grid[numRows][numCols], Coord start) {
  uint64 beams[numCols]; // holds multiple beams per column
  memset(beams, 0, numCols*sizeof(uint64)); // initialize to 0
  beams[start.c] = 1;
  for (int row = 2; row < numRows; row+=2) {
    for (int col = 1; col < numCols-1; col++) {
      // split
      if (grid[row][col] == '^') {
        beams[col-1] += beams[col];
        beams[col+1] += beams[col];
        beams[col] = 0;
      }
    }
  }
  // count all quantum beams
  uint64 total = 0;
  for (int i = 0; i < numCols; i++) {
    total += beams[i];
  }
  return total;
}
