#include "sbd.h"

int tape_allocs = 0;
int run_allocs = 0;
int value_allocs = 0;
int tape_frees = 0;
int run_frees = 0;
int value_frees = 0;

void print_tape(Tape *tape)
{
	Run *run = tape->first;

	while (run != NULL)
	{
		Value *val = run->first;
		while (val != NULL)
		{
			printf("%s, %s\n", val->last_name, val->first_name);
			val = val->next;
		}
		printf("\n");
		run = run->next;
	}
}

Run *get_last_run(Tape *tape)
{
	Run *last_run;

	if (tape == NULL)
	{
		return NULL;
	}

	if (tape->first == NULL)
	{
		return NULL;
	}

	last_run = tape->first;
	while (last_run->next != NULL)
	{
		last_run = last_run->next;
	}
	return last_run;
}

Value *get_last_value(Tape *tape)
{
	Value *last_value;
	Run *last_run = get_last_run(tape);

	if (last_run == NULL)
	{
		return NULL;
	}

	last_value = last_run->first;
	while (last_value->next != NULL)
	{
		last_value = last_value->next;
	}
	return last_value;
}

int compare_values(Value *a, Value *b)
{
	int last_name_cmp = memcmp(a->last_name, b->last_name, LAST_NAME_LEN);
	if (last_name_cmp < 0) {
		return -1;
	}
	else if (last_name_cmp == 0) {

		int first_name_cmp = memcmp(a->first_name, b->first_name, FIRST_NAME_LEN);
		if (first_name_cmp < 0) {
			return -1;
		} else if (first_name_cmp == 0) {
			return 0;
		}
	}
	return 1;
}

void add_name(Tape *tape, char *last_name, char *first_name)
{
	if (tape == NULL) {
		return;
	}

	Run *last_run = get_last_run(tape);
	Value *new_value = (Value *)calloc(1, sizeof(Value));
	new_value->first_name = first_name;
	new_value->last_name = last_name;

	value_allocs++;

	if (last_run == NULL)
	{
		Run *new_run = (Run *)calloc(1, sizeof(Run));
		new_run->first = new_value;
		tape->first = new_run;

		run_allocs++;
		return;
	}

	Value *last_value = get_last_value(tape);
	int comp = compare_values(last_value, new_value);
	if(comp <= 0) {
		last_value->next = new_value;
		return;
	}

	Run *new_run = (Run *)calloc(1, sizeof(Run));
	new_run->first = new_value;
	last_run->next = new_run;

	run_allocs++;
}

void free_run(Run *run)
{
	Value *to_free, *next;

	if (run->first == NULL)
	{
		free(run);
		run_frees++;
		return;
	}

	to_free = run->first;
	if (to_free->next == NULL)
	{
		free(to_free);
		free(run);
		value_frees++;
		run_frees++;
		return;
	}

	next = to_free->next;
	while (next->next != NULL)
	{
		free(to_free);
		value_frees++;
		to_free = next;
		next = to_free->next;
	}

	free(to_free);
	free(next);
	value_frees+=2;
	free(run);
	run_frees++;
}

void free_tape(Tape *tape)
{
	Run *to_free, *next;

	if (tape->first == NULL)
	{
		printf("%d tape free\n", tape);
		free(tape);
		tape_frees++;
		return;
	}

	to_free = tape->first;
	if (to_free->next == NULL)
	{
		free_run(to_free);
		printf("%d tape free\n", tape);
		free(tape);
		tape_frees++;
		return;
	}

	next = to_free->next;
	while (next->next != NULL)
	{
		free_run(to_free);
		to_free = next;
		next = to_free->next;
	}

	free_run(to_free);
	free_run(next);
	free(tape);
	printf("%d tape free\n", tape);
	tape_frees++;
}

Run *merge_runs(Run *run1, Run *run2)
{

	if (run1 == NULL && run2 == NULL)
	{
		return NULL;
	}
	if (run1 == NULL)
	{
		return run2;
	}
	if (run2 == NULL)
	{
		return run1;
	}

	Run *new_run = (Run *)calloc(1, sizeof(Run));
	Value *last_value;
	int name_cmp;

	run_allocs++;

	name_cmp = compare_values (run1->first, run2->first);
	if (name_cmp <= 0)
	{
		new_run->first = run1->first;
		run1->first = run1->first->next;
	}
	else
	{
		new_run->first = run2->first;
		run2->first = run2->first->next;
	}
	new_run->first->next = NULL;

	last_value = new_run->first;
	while (run1->first != NULL && run2->first != NULL)
	{
		name_cmp = compare_values (run1->first, run2->first);
		if (name_cmp <= 0)
		{
			last_value->next = run1->first;
			run1->first = run1->first->next;
		}
		else
		{
			last_value->next = run2->first;
			run2->first = run2->first->next;
		}
		last_value = last_value->next;
		last_value->next = NULL;
	}

	Run *remainder = run1->first == NULL ? run2 : run1;
	while (remainder->first != NULL)
	{
		last_value->next = remainder->first;
		remainder->first = remainder->first->next;
		last_value = last_value->next;
		last_value->next = NULL;
	}
	free_run(run1);
	free_run(run2);
	return new_run;
}

Tape *sort_tape(Tape *tape)
{
	if (tape == NULL)
	{
		return NULL;
	}

	if (tape->first == NULL)
	{
		return tape;
	}

	if (tape->first->next == NULL)
	{
		return tape;
	}

	Tape *tape1 = (Tape *)calloc(1, sizeof(Tape));
	Tape *tape2 = (Tape *)calloc(1, sizeof(Tape));
	tape_allocs+=2;
	int i = 0;

	tape1->first = tape->first;
	tape->first = tape->first->next;
	tape1->first->next = NULL;

	tape2->first = tape->first;
	tape->first = tape->first->next;
	tape2->first->next = NULL;

	if (tape != NULL)
	{
		while (tape->first != NULL)
		{
			Run *run;
			if(!i) {
				run = get_last_run(tape1);
			} else {
				run = get_last_run(tape2);
			}
			run->next = tape->first;
			tape->first = tape->first->next;
			run->next->next = NULL;
			i = !i;
		}
	}
	tape = merge_tapes(tape1, tape2);

	return tape;
}

Tape * merge_tapes(Tape *tape1, Tape *tape2)
{
	if (tape1 == NULL && tape2 == NULL) {
		return NULL;
	}
	if (tape1 == NULL) {
		return tape2;
	}
	if (tape2 == NULL) {
		return tape1;
	}

	Tape *new_tape = (Tape *)calloc(1, sizeof(Tape));
	tape_allocs++;
	new_tape->first = merge_runs(tape1->first, tape2->first);
	tape1->first = tape1->first->next;
	tape2->first = tape2->first->next;
	while (tape1->first != NULL && tape2->first != NULL)
	{
		Run *next_run = get_last_run(new_tape);
		next_run->next = merge_runs(tape1->first, tape2->first);
		tape1->first = tape1->first->next;
		tape2->first = tape2->first->next;
	}
	Run *next_run = get_last_run(new_tape);
	next_run->next = merge_runs(tape1->first, tape2->first);

	return new_tape;
}

void print_out_file()
{
	FILE *file;
	char *f_name = (char *)calloc(FIRST_NAME_LEN + 1, sizeof(char));
	char *l_name = (char *)calloc(LAST_NAME_LEN + 1, sizeof(char));

	file = fopen("names.data", "r");
	f_name[FIRST_NAME_LEN] = '\0';
	l_name[LAST_NAME_LEN] = '\0';

	while (1) {
		int read_bytes;

		read_bytes = fread(l_name, sizeof(char), LAST_NAME_LEN, file);
		if (!read_bytes) {
			break;
		}

		read_bytes = fread(f_name, sizeof(char), FIRST_NAME_LEN, file);
		if (!read_bytes) {
			break;
		}

		printf("%s %s\n", l_name, f_name);
	}

	free(f_name);
	free(l_name);
	fclose(file);
}

Tape * load_file() {
	Tape *t1 = (Tape*)calloc(1, sizeof(Tape));
	FILE *file;
	

	file = fopen("names.data", "r");
	

	while (1) {
		int read_bytes; 
		char *f_name = (char *)calloc(FIRST_NAME_LEN + 1, sizeof(char));
		char *l_name = (char *)calloc(LAST_NAME_LEN + 1, sizeof(char));
		f_name[FIRST_NAME_LEN] = '\0';
		l_name[LAST_NAME_LEN] = '\0';

		read_bytes = fread(l_name, sizeof(char), LAST_NAME_LEN, file);
		if (!read_bytes) {
			break;
		}

		read_bytes = fread(f_name, sizeof(char), FIRST_NAME_LEN, file);
		if (!read_bytes) {
			break;
		}

		add_name(t1, l_name, f_name);
	}

	fclose(file);
	return t1;
}

void save_tape_to_file(Tape *tape) {
  FILE *file;
  file = fopen("test", "w");
  char *string = (char *)calloc(FIRST_NAME_LEN + 1, sizeof(char));
  string[FIRST_NAME_LEN] = '\0';

  Run *current_run = tape->first;
  Value *current_value = current_run->first;

  while (current_run != NULL) {
    current_value = current_run->first;
    while (current_value != NULL) {
      memset (string, 0, FIRST_NAME_LEN);
      strcpy (string, current_value->last_name);
      fwrite (string, sizeof(char), LAST_NAME_LEN, file);
      memset (string, 0, FIRST_NAME_LEN);
      strcpy (string, current_value->first_name);
      fwrite (string, sizeof(char), FIRST_NAME_LEN, file);
      current_value = current_value->next;
    }
    current_run = current_run->next;
  }

  fclose(file);
}

int main()
{
  Tape *tape = load_file();

  int num = 0;
  printf("Before sorting:\n");
	print_tape(tape);
  while(tape->first->next != NULL) {
    num++;
	  tape = sort_tape(tape);
    printf("%d sorts:\n", num);
	  print_tape(tape);
  }
  save_tape_to_file(tape);

	return 0;
}
