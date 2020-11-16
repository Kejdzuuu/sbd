#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define FIRST_NAME_LEN 10
#define LAST_NAME_LEN 20

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

int compare_values(char *a, char *b)
{
	int last_name_cmp = memcmp(a, b, LAST_NAME_LEN);
	if (last_name_cmp < 0)
	{
		return
	}
	else if (last_name_cmp == 0)
	{

		int first_name_cmp = memcmp(last_value->first_name, new_value->first_name, FIRST_NAME_LEN);
		if (first_name_cmp <= 0)
		{
			last_value->next = new_value;
			return;
		}
	}
}

void add_name(Tape *tape, char *first_name, char *last_name)
{
	Run *last_run = get_last_run(tape);
	Value *new_value = (Value *)calloc(1, sizeof(Value));
	new_value->first_name = first_name;
	new_value->last_name = last_name;

	if (last_run == NULL)
	{
		Run *new_run = (Run *)calloc(1, sizeof(Run));
		new_run->first = new_value;
		tape->first = new_run;
		return;
	}

	Value *last_value = get_last_value(tape);
	int last_name_cmp = memcmp(last_value->last_name, new_value->last_name, LAST_NAME_LEN);
	if (last_name_cmp < 0)
	{
		last_value->next = new_value;
		return;
	}
	else if (last_name_cmp == 0)
	{

		int first_name_cmp = memcmp(last_value->first_name, new_value->first_name, FIRST_NAME_LEN);
		if (first_name_cmp <= 0)
		{
			last_value->next = new_value;
			return;
		}
	}

	Run *new_run = (Run *)calloc(1, sizeof(Run));
	new_run->first = new_value;
	last_run->next = new_run;
}

void free_run(Run *run)
{
	Value *to_free, *next;

	if (run->first == NULL)
	{
		free(run);
		return;
	}

	to_free = run->first;
	if (to_free->next == NULL)
	{
		free(to_free);
		free(run);
		return;
	}

	next = to_free->next;
	while (next->next != NULL)
	{
		free(to_free);
		to_free = next;
		next = to_free->next;
	}

	free(to_free);
	free(next);
	free(run);
}

void free_tape(Tape *tape)
{
	Run *to_free, *next;

	if (tape->first == NULL)
	{
		free(tape);
		return;
	}

	to_free = tape->first;
	if (to_free->next == NULL)
	{
		free_run(to_free);
		free(tape);
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
}

Run *merge_runs(Run *run1, Run *run2)
{
	Run *new_run = (Run *)calloc(1, sizeof(Run));
	Value *last_value;
	int last_name_cmp;
	int first_name_cmp;

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

	if (run1->first->num < run2->first->num)
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
		if (run1->first->num < run2->first->num)
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

	Tape temp_tapes[2];
	int i = 0;

	temp_tapes[0].first = tape->first;
	tape->first = tape->first->next;
	temp_tapes[0].first->next = NULL;

	temp_tapes[1].first = tape->first;
	tape->first = tape->first->next;
	temp_tapes[1].first->next = NULL;

	if (tape != NULL)
	{
		while (tape->first != NULL)
		{
			Run *run = get_last_run(&temp_tapes[i]);
			run->next = tape->first;
			tape->first = tape->first->next;
			run->next->next = NULL;
			i = !i;
		}
	}
	//tape->first = merge_runs(temp_tapes[0].first, temp_tapes[1].first);
	temp_tapes[0].first = temp_tapes[0].first->next;
	temp_tapes[1].first = temp_tapes[1].first->next;
	while (temp_tapes[0].first != NULL && temp_tapes[1].first != NULL)
	{
		Run *next_run = get_last_run(tape);
		//next_run->next = merge_runs(temp_tapes[0].first, temp_tapes[1].first);
		temp_tapes[0].first = temp_tapes[0].first->next;
		temp_tapes[1].first = temp_tapes[1].first->next;
	}

	return tape;
}

int main()
{
	/*int arr[] = { 44, 55, 12, 42, 94, 18, 06, 67 };
	Tape *t1 = (Tape*)calloc(1, sizeof(Tape));

	for (int i = 0; i < sizeof(arr) / sizeof(int); i++) {
		add_num(t1, arr[i]);
	}

	print_tape(t1);

	sort_tape(t1);

	print_tape(t1);
	sort_tape(t1);

	print_tape(t1);

	free_tape(t1);*/

	char a[10] = "malaga";
	char b[10] = "aslaga";

	int result = memcmp(a, b, 6);

	if (result > 0)
	{
		printf("%s\n%s\n", b, a);
	}
	else if (result < 0)
	{
		printf("%s\n%s\n", a, b);
	}
	else
	{
		printf("ruwne\n");
	}

	return 0;
}
