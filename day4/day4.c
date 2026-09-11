#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#define PRINT_ROUNDS true

typedef struct Coord {
  int row;
  int col;
} Coord;

typedef struct Node {
  Coord *coord;
  struct Node *next;
} Node;

typedef struct CoordList {
  Node *head;
  Node *tail;
  int len;
} CoordList;

int removeRolls(const int numRows, const int numCols, char grid[numRows][numCols]);
int checkAdjacentSpaces(const int numRows, const int numCols, const char grid[numRows][numCols], const int i, const int j);
void printGrid(const int numCols, const int numRows, const char grid[numCols][numRows]);
int findLen(const char* str);
CoordList *constructCoordList();
Node* setNodeCoord(CoordList *l, const int n, const int r, const int c);
Node* addNode(CoordList *l, const int r, const int c);
Node* allocNode();


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

  // get grid dimensions
  char line[140];
  fgets(line, 140, file);
  const int numCols = findLen(line);
  int numRows = 1;
  while (fgets(line, 140, file) != NULL) {
    numRows++;
  }
  fseek(file, 0, SEEK_SET);

  printf("Dimensions: x=%d, y=%d\n", numCols, numRows);

  // read in grid info 
  char grid[numRows][numCols];
  for (int i = 0; i < numRows; i++) {
    fgets(line, 140, file);
    strncpy(&grid[i][0], line, numCols); // cleans \n and \0
  }
  fclose(file);

  if (PRINT_ROUNDS) {
    printf("\n");
    puts("Input:");
    printGrid(numCols, numRows, grid);
    printf("\n");
  }

  int rndNum = 1;
  int pt1Cnt;
  int pt2Cnt = 0;
  int removed = -1;
  while (removed != 0) {
    if (PRINT_ROUNDS) {
      printf("\n");
      printf("Round %d:\n", rndNum);
    }
    removed = removeRolls(numCols, numRows, grid);
    if (rndNum == 1) pt1Cnt = removed; // Part 1 only counts round 1
    pt2Cnt += removed;
    rndNum++;
  }

  printf("PART 1 ANSWER: %d\n", pt1Cnt); 
  printf("PART 2 ANSWER: %d\n", pt2Cnt); 

  return 0;
}

int removeRolls(const int numRows, const int numCols, char grid[numRows][numCols]) {
  CoordList *toRemove = constructCoordList(); 
  int removed = 0;
  for (int i = 0; i < numRows; i++) {
    for (int j = 0; j < numCols; j++) {
      if (grid[i][j] == '@') {
        int neighbors = checkAdjacentSpaces(numRows, numCols, grid, i, j);
        if (neighbors < 4) {
          // remove roll
          removed++;
          grid[i][j] = 'x';
          addNode(toRemove, i, j);
          // printf("%d,%d\n",i,j);
        }
      }
    }
  }
  if (PRINT_ROUNDS) {
    printGrid(numCols, numRows, grid);
    printf("\n");
  }

  // clean up Xs for next round
  Node* curr = toRemove->head;
  while (curr != NULL) {
    const int r = curr->coord->row;
    const int c = curr->coord->col;
    grid[r][c] = '.';
    curr = curr->next;
  }

  return removed;
}

int checkAdjacentSpaces(const int numRows, const int numCols, const char grid[numRows][numCols], const int i, const int j) {
  // @ -> current roll
  // x -> roll to be removed this round

  int neighbors = 0;
  // top
  if (i > 0) {
    // top middle
    if (grid[i-1][j] == '@' || grid[i-1][j] == 'x') neighbors++;
    // top right
    if (j < numCols-1 && (grid[i-1][j+1] == '@' || grid[i-1][j+1] == 'x')) neighbors++;
    // top left
    if (j > 0 && (grid[i-1][j-1] == '@' || grid[i-1][j-1] == 'x')) neighbors++;

  }
  // bottom
  if (i < numRows-1) {
    // bottom middle
    if (grid[i+1][j] == '@' || grid[i+1][j] == 'x') neighbors++;
    // bottom right
    if (j < numCols-1 && (grid[i+1][j+1] == '@' || grid[i+1][j+1] == 'x')) neighbors++;
    // bottom left
    if (j > 0 && (grid[i+1][j-1] == '@' || grid[i+1][j-1] == 'x')) neighbors++;
  }
  // left
  if (j > 0 && (grid[i][j-1] == '@' || grid[i][j-1] == 'x')) neighbors++;
  // right
  if (j < numCols-1 && (grid[i][j+1] == '@' || grid[i][j+1] == 'x')) neighbors++;

  return neighbors;
}

void printGrid(const int numCols, const int numRows, const char grid[numCols][numRows]) {
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

CoordList *constructCoordList() {
  CoordList *list = malloc(sizeof(CoordList));
  list->head = NULL;
  list->tail = NULL;
  list->len = 0;
  return list;
}

Node* setNodeCoord(CoordList *l, const int n, const int r, const int c) {
  if (n >= l->len || n < 0) {
    fprintf(stderr, "Argument n is out of range of list length :(\n");
    exit(1);
  }
  Node* curr = l->head;
  for (int i = 0; i < n; i++) {
    curr = curr->next;
  }
  curr->coord->row = r;
  curr->coord->col = c;
  return curr;
}

Node* addNode(CoordList *l, const int r, const int c) {
  Node* curr = l->head;
  // if list empty make first node
  if (curr == NULL) {
    curr = allocNode();
    curr->coord->row = r;
    curr->coord->col = c;
    l->head = curr;
    l->tail = curr;
    l->len = 0;
    return curr;
  }
  // otherwise add to tail
  curr = l->tail;
  curr->next = allocNode();
  curr = curr->next;
  curr->coord->row = r;
  curr->coord->col = c;
  l->tail = curr;
  l->len++;
  return curr;
}

Node* allocNode() {
  Node *n = malloc(sizeof(Node));
  n->coord = malloc(sizeof(Coord));
  n->next = NULL;
}
