#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define FIRST_NAME_LEN 16
#define LAST_NAME_LEN 16

struct Value
{
	char *first_name;
	char *last_name;
	Value *next = NULL;
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