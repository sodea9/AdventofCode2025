#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#define PRINT_LISTS false

// ranges are inclusive
typedef struct Range {
  unsigned long long min;
  unsigned long long max;
} Range;

typedef struct Node {
  Range *range;
  struct Node *next;
} Node;

typedef struct RangeList {
  Node *head;
  Node *tail;
  int len;
} RangeList;

void mergeRanges(RangeList *rList, const unsigned long long lower, const unsigned long long upper);
bool rangesEqual(const Range* r1, const Range* r2);
void printList(const RangeList* l);
RangeList *constructRangeList();
void destructRangeList(RangeList *l);
Node* setNodeRange(RangeList *l, const int n, const unsigned long long min, const unsigned long long max);
Node* addNode(RangeList *l, const unsigned long long min, const unsigned long long max);
Node* allocNode();
void destructNode(Node* n);


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

  char line[33];
  RangeList *list = constructRangeList();
  // for each range add it to a list
  while (fgets(line, 33, file)[0] != '\n') {
    if (PRINT_LISTS) printList(list);
    char *endPtr;
    unsigned long long min = strtoll(line, &endPtr, 10);
    unsigned long long max = strtoll(endPtr+1, NULL, 10);
    mergeRanges(list, min, max);
  }
  int freshCnt = 0;
  unsigned long long pt2Answer;
  // for each id check if it fits in a range
  while (fgets(line, 32, file) != NULL) {
    unsigned long long id = strtoll(line, NULL, 10);
    Node *curr = list->head;
    unsigned long long pt2Cnt = 0;
    while (curr) {
      Range *r = curr->range;
      if (r->min <= id && id <= r->max) {
        freshCnt++;
        break;
      }
      pt2Cnt += (r->max - r->min) + 1;
      curr = curr->next;
    }
    pt2Answer = pt2Cnt;
  }
  fclose(file);
  destructRangeList(list);

  printf("PART 1 ANSWER: %d\n", freshCnt); 
  printf("PART 2 ANSWER: %llu\n", pt2Answer); 

  return 0;
}

// mergeRanges: adds new range to existing list such that none will be overlapping
// existing range: min-max
// new range: lower-upper
//
// assumption: at any point all ranges in given list are non overlapping
// for any existing range the new one can:
//   be fully covered - min < lower && upper < max
//   cover it fully - lower < min && max < upper
//   hang off the left - lower < min && min < upper && upper < max
//   hang off the right - min < lower && lower < max && max < upper
//   not overlap - else
//
// if new range is fully covered already we ignore it
// if it only hangs off the left of an exising one, extend that range downwards
// if it only hangs off the right of an existing one, extend that range upwards
// if it bridges two existing ranges (hangs off left and right respectively), extend the lower one and delete the higher one
// if new range bounds are not directly included in an existing range, add it to the list
//            (can occur if it eclipses other ranges or is simply separate from all others)
// delete from the list all existing ranges that have been fully eclipsed by the new one (may occur in the above 3 cases or otherwise)
void mergeRanges(RangeList *rList, const unsigned long long lower, const unsigned long long upper) {
  RangeList *toDelete = constructRangeList();
  bool hangLeft = false, hangRight = false;
  Range *hl, *hr;
  Node *curr = rList->head;
  // for each range classify if and how it overlaps with existing ranges
  while (curr) {
    const unsigned long long min = curr->range->min;
    const unsigned long long max = curr->range->max;
    if (min <= lower && upper <= max) {
      // MIN             MAX
      //     LOWER UPPER
      return;
    } if (lower < min && min <= upper && upper <= max) {
      //       MIN       MAX
      // LOWER     UPPER
      hangLeft = true;
      hl = curr->range;
    } if (min <= lower && lower <= max && max < upper) {
      // MIN       MAX
      //     LOWER     UPPER
      hangRight = true;
      hr = curr->range;
    } if (lower < min && max < upper) {
      //       MIN MAX
      // LOWER         UPPER
      addNode(toDelete, min, max);
    }
    curr = curr->next;
  }

  // modify existing ranges as needed
  if (hangLeft && !hangRight) {
    hl->min = lower; // extend hl
  } else if (!hangLeft && hangRight) {
    hr->max = upper; // extend hr
  } else if (hangLeft && hangRight) {
    hl->min = hr->min;
    addNode(toDelete, hr->min, hr->max); // extend hl, delete hr
  } else {
    // no overlap
    addNode(rList, lower, upper);
  }

  // delete all ranges encompassed by newly merged ones
  Node *del = toDelete->head;
  while (del) {
    curr = rList->head;
    Node *prev = NULL;
    while (curr) {
      if (rangesEqual(curr->range, del->range)) {
        // delete from rList
        if (curr->next == NULL) rList->tail = prev; // update tail ptr
        if (prev) { // not the head
          prev->next = curr->next;
          destructNode(curr);
          curr = prev->next;
        } else { // head of list
          rList->head = curr->next;
          destructNode(curr);
          curr = rList->head;
        }
        rList->len--;
        break;
      } else {
        prev = curr;
        curr = curr->next;
      }
    }
    del = del->next;
  }
}

bool rangesEqual(const Range* r1, const Range* r2) {
  return r1->min == r2->min && r1->max == r2->max;
}

void printList(const RangeList* l) {
  Node* curr = l->head;
  while (curr) {
    printf("%llu-%llu, ", curr->range->min, curr->range->max);
    curr = curr->next;
  }
  printf("\n");
}

RangeList *constructRangeList() {
  RangeList *list = malloc(sizeof(RangeList));
  list->head = NULL;
  list->tail = NULL;
  list->len = 0;
  return list;
}

void destructRangeList(RangeList *l) {
  Node* curr = l->head;
  while (curr) {
    free(curr->range);
    Node *next = curr->next;
    free(curr);
    curr = next;
  }
  free(l);
}

Node* setNodeRange(RangeList *l, const int n, const unsigned long long min, const unsigned long long max) {
  if (n >= l->len || n < 0) {
    fprintf(stderr, "Argument n is out of range of list length :(\n");
    exit(1);
  }
  Node* curr = l->head;
  for (int i = 0; i < n; i++) {
    curr = curr->next;
  }
  curr->range->min = min;
  curr->range->max = max;
  return curr;
}

Node* addNode(RangeList *l, const unsigned long long min, const unsigned long long max) {
  Node* curr = l->head;
  // if list empty make first node
  if (curr == NULL) {
    curr = allocNode();
    curr->range->min = min;
    curr->range->max = max;
    l->head = curr;
    l->tail = curr;
    l->len = 1;
    return curr;
  }
  // otherwise add to tail
  curr = l->tail;
  curr->next = allocNode();
  curr = curr->next;
  curr->range->min = min;
  curr->range->max = max;
  l->tail = curr;
  l->len++;
  return curr;
}

Node* allocNode() {
  Node *n = malloc(sizeof(Node));
  n->range = malloc(sizeof(Range));
  n->next = NULL;
}

void destructNode(Node* n) {
  free(n->range);
  free(n);
}
