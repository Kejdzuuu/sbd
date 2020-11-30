#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define FIRST_NAME_LEN 16
#define LAST_NAME_LEN 16
#define PAGE_SIZE 4096
#define RECORDS_IN_PAGE 15 //128
#define BUFFERS_AVAILABLE 30 //100
#define MAX_RUN (RECORDS_IN_PAGE * BUFFERS_AVAILABLE)

const char tape_file1[] = "tape1.tp";
const char tape_file2[] = "tape2.tp";
const char tape_file3[] = "tape3.tp";

int run_count_global;

struct Value
{
  char *first_name;
  char *last_name;
  Value *next = NULL;
};

struct Record
{
  char last_name[LAST_NAME_LEN];
  char first_name[FIRST_NAME_LEN];
};

struct Record_with_index
{
  Record *record;
  int index;
};

struct Run
{
  Value *first = NULL;
  Run *next = NULL;
};

struct Tape
{
  Run *first = NULL;
};

Run * merge_runs(Run *run1, Run *run2);

Tape * sort_tape(Tape *tape);

Tape * merge_tapes(Tape *tape1, Tape *tape2);

void save_tape_to_file(Tape *tape, const char name[]);

void load_file(const char name[]);

int merge_runs(int runs);