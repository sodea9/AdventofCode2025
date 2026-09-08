#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#define FACTOR_LIST_LEN 11

typedef struct FactorPair {
  int segCount;
  int segLen;
} FactorPair;

typedef struct Node {
  FactorPair *pair;
  struct Node *next;
} Node;

typedef Node** FactorList;

unsigned long long sumPt1(char *lower, int lowerLen, char *upper, int upperLen);
unsigned long long sumPt2(char *lower, int lowerLen, char *upper, int upperLen, FactorList fList);
int numDigits(unsigned int x);
bool isRepdigit(int seg);
FactorList constructFactorList();
Node *allocateNode(int count, int len);
void destructFactorList();

// apparently integers whose digits are all equal are called "repdigits" (https://en.wikipedia.org/wiki/Repdigit)
// although I prefer to call them "gleichstellig" which is a pseudo-German word that feels more eloquent

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

  // read input
  char input[528];
  fgets(input, 528, file);
  fclose(file);

  puts("Invalid IDs:");

  unsigned long long invalidSumPt1 = 0;
  unsigned long long invalidSumPt2 = 0;
  char *range;
  FactorList fList = constructFactorList();
  // for each range
  for (range = strtok(input, ","); range && *range; range = strtok(NULL, ",")) {
    // extract limits
    char lower[16]; // range lower limit
    char upper[16]; // range upper limit
    int lowerLen = 0;
    int upperLen = 0;

    //   lower
    int i = 0;
    while (range[i] != '-') {
      lower[i] = range[i];
      i++;
      lowerLen++;
    }
    lower[i] = '\0';
    i++;

    //   upper
    int iStart = i;
    while (range[i] != '\0' && range[i] != '\n') {
      upper[i-iStart] = range[i];
      i++;
      upperLen++;
    }
    upper[i-iStart] = '\0';

    // check invalids
    invalidSumPt1 += sumPt1(lower, lowerLen, upper, upperLen);
    invalidSumPt2 += sumPt2(lower, lowerLen, upper, upperLen, fList);
  }

  destructFactorList(fList);

  printf("PART 1 ANSWER: %llu\n", invalidSumPt1); 
  printf("PART 2 ANSWER: %llu\n", invalidSumPt2); 

  return 0;
}

unsigned long long sumPt1(char *lower, int lowerLen, char *upper, int upperLen) {
  /* ---------------- OLD METHOD ----------------------
  * (slightly more efficient (by a coefficient not an order of n) but technically incorrect, although the puzzle input actually doesn't catch the error)
    // case 1:
    if (lowerLen == upperLen && lowerLen % 2 == 1) continue;

    // case 2:
    // convert strs to numbers
    unsigned long long lowerNum = strtoll(lower, NULL, 10);
    unsigned long long upperNum = strtoll(upper, NULL, 10);

    // extract first half range (str)
    char firstHalfLower[8]; // first half of lower limit
    char firstHalfUpper[8]; // first half of upper limit
    // if odd length for lower limit length do floor division
    strncpy(firstHalfLower, lower, lowerLen/2); 
    // if odd length for upper limit length do ceiling division
    int cpyLen = upperLen % 2 == 0 ? upperLen / 2 : upperLen / 2 + 1; 
    strncpy(firstHalfUpper, upper, cpyLen);
    firstHalfLower[lowerLen/2] = '\0'; // always null-terminate :)
    firstHalfUpper[cpyLen] = '\0';

    // convert str to int
    unsigned int firstHalfLowerNum = strtol(firstHalfLower, NULL, 10);
    unsigned int firstHalfUpperNum = strtol(firstHalfUpper, NULL, 10);

    // iterate over first half range (int)
    for (int x = firstHalfLowerNum; x <= firstHalfUpperNum; x++) {
      // create doubled version of each candidate (str)
      int xLen = numDigits(x);
      char candidate[16];
      snprintf(candidate, 16, "%d\n", x);
      snprintf(candidate+xLen, 16-xLen, "%d\n", x);
      // check if within range (int)
      unsigned long long candidateNum = strtoll(candidate, NULL, 10);
      if (candidateNum >= lowerNum && candidateNum <= upperNum) {
        printf("%d\n", candidateNum);
        invalidSum += candidateNum;
      }
    }
    ---------------------------------------------------- */

  // ------------------- NEW METHOD --------------------
  // (slightly less efficient but much simpler)
  unsigned long long lowerNum = strtoll(lower, NULL, 10);
  unsigned long long upperNum = strtoll(upper, NULL, 10);
  unsigned long long invalidSum = 0;

  // for all even digits in range lower len to upper len
  for (int len = lowerLen; len <= upperLen; len++) {
    if (len % 2 != 0) continue;

    // calculate all possible invalid ids with that many digits
    // find all n/2 digit numbers and concatenate so it repeats
    int segLen = len/2;
    int segMin = pow(10, segLen-1);
    int segMax = 9;
    for (int x = 0; x < segLen-1; x++) {
      segMax *= 10;
      segMax += 9;
    }
    for (int seg = segMin; seg <= segMax; seg++) {
      char candidate[16];
      snprintf(candidate, 16, "%d\n", seg);
      snprintf(candidate+segLen, 16-segLen, "%d\n", seg);
      // check if within range
      unsigned long long candidateNum = strtoll(candidate, NULL, 10);
      if (candidateNum >= lowerNum && candidateNum <= upperNum) {
        // printf("%llu\n", candidateNum);
        invalidSum += candidateNum;
      }
    }
  }
  return invalidSum;
  // -----------------------------------------------------------
}

unsigned long long sumPt2(char *lower, int lowerLen, char *upper, int upperLen, FactorList fList) {
  unsigned long long lowerNum = strtoll(lower, NULL, 10);
  unsigned long long upperNum = strtoll(upper, NULL, 10);
  unsigned long long invalidSum = 0;
  
  // for each possible id length
  for (int len = lowerLen; len <= upperLen; len++) {
    if (upperLen > 10) {
      fprintf(stderr, "range upper limit length too high (max 10)\n");
      exit(1);
    }
    // combinatoric things
    // split length into factor pairs (COUNT x LENGTH), see `constructFactorList`
    bool repdigitUsed[] = {0,0,0,0,0,0,0,0,0,0}; // track if repdigit already used for each digit
    Node *head = fList[len];
    // for each pair
    while (head != NULL) {
      int segCount = head->pair->segCount;
      int segLen = head->pair->segLen;
      // find segment min and max
      int segMin = pow(10, segLen-1);
      int segMax = 9;
      for (int x = 0; x < segLen-1; x++) {
        segMax *= 10;
        segMax += 9;
      }
      // construct all possible tilings of total length
      for (int seg = segMin; seg <= segMax; seg++) {
        char candidate[16];
        // check if seg is repdigit to avoid overlap
        if (isRepdigit(seg)) {
          int digit = seg % 10;
          if (repdigitUsed[digit]) continue; 
          else repdigitUsed[digit] = true;
        }
        for (int i = 0; i < segCount; i++) {
          snprintf(candidate+i*segLen, 16, "%d\n", seg);
        }
        // check if within range
        unsigned long long candidateNum = strtoll(candidate, NULL, 10);
        if (candidateNum >= lowerNum && candidateNum <= upperNum) {
          printf("%llu\n", candidateNum);
          invalidSum += candidateNum;
        }
      }
      head = head->next;
    }
  }
  return invalidSum;
}

// find number of digit places in an integer
int numDigits(unsigned int x) {
  int digits = 0;
  do {
    x /= 10;
    digits++;
  } while (x > 0);
  return digits;
}

// check if all digits equal
bool isRepdigit(int seg) {
  int digit = seg % 10;
  while (seg != 0) {
    if (seg % 10 != digit) return false;
    seg /= 10;
  }
  return true;
}

// factor pair list by index:
// COUNT x LENGTH
// 0  N/A
// 1  N/A
// 2  2x1
// 3  3x1
// 4  2x2
// 5  5x1 
// 6  2x3 3x2
// 7  7x1
// 8  2x4
// 9  3x3
// 10 5x2 2x5
// there's probably a smart algorithm to find this but seemed easier to just use a lookup table for lengths used in this puzzle
// ignores pairs where COUNT=1, uses largest coprime values for length (this is to avoid overlaps -- except those where all digits are the same which is always present, ex: all segments of length 2 are covered by segments of length 4)
// there's definitely a more mathematically precise way to define this
FactorList constructFactorList() {
  FactorList factorList;
  if (!(factorList = (FactorList) calloc(FACTOR_LIST_LEN, sizeof(Node*)))) {
    fprintf(stderr, "Dynamic memory alloc failed :(\n");
    exit(1);
  }
  factorList[0] = NULL;
  factorList[1] = NULL;
  factorList[2] = allocateNode(2,1);
  factorList[3] = allocateNode(3,1);
  factorList[4] = allocateNode(2,2);
  factorList[5] = allocateNode(5,1);
  factorList[6] = allocateNode(3,2);
  factorList[6]->next = allocateNode(2,3);
  factorList[7] = allocateNode(7,1);
  factorList[8] = allocateNode(2,4);
  factorList[9] = allocateNode(3,3);
  factorList[10] = allocateNode(5,2);
  factorList[10]->next = allocateNode(2,5);

  return factorList;
}

Node *allocateNode(int count, int len) {
  Node *newNode;
  FactorPair *newPair;
  if (!(newNode = (Node*) malloc(sizeof(Node)))) {
    fprintf(stderr, "Dynamic memory alloc failed :(\n");
    exit(1);
  }
  if (!(newPair = (FactorPair*) malloc(sizeof(FactorPair)))) {
    fprintf(stderr, "Dynamic memory alloc failed :(\n");
    exit(1);
  }
  newNode->pair = newPair;
  newNode->next = NULL;
  newPair->segCount = count;
  newPair->segLen = len;
  return newNode;
}

void destructFactorList(FactorList list) {
  for (int i = 0; i < FACTOR_LIST_LEN; i++) {
    Node *head = list[i];
    while (head != NULL) {
      Node *next = head->next;
      free(head->pair);
      free(head);
      head = next;
    }
  }
  free(list);
}
