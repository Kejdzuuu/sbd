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

int compare_values(Value *a, Value *b) {
  return -1;
}


int compare_records(Record *a, Record *b)
{
  int last_name_cmp = memcmp(a->last_name, b->last_name, LAST_NAME_LEN);
  if (last_name_cmp < 0) {
    return -1;
  }
  else if (last_name_cmp == 0) {

    int first_name_cmp = memcmp(a->first_name, b->first_name, FIRST_NAME_LEN);
    if (first_name_cmp < 0) {
      return -1;
    }
    else if (first_name_cmp == 0) {
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
  if (comp <= 0) {
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
  value_frees += 2;
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

  name_cmp = compare_values(run1->first, run2->first);
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
    name_cmp = compare_values(run1->first, run2->first);
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
  tape_allocs += 2;
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
      if (!i) {
        run = get_last_run(tape1);
      }
      else {
        run = get_last_run(tape2);
      }
      run->next = tape->first;
      tape->first = tape->first->next;
      run->next->next = NULL;
      i = !i;
    }
  }
  //save_tape_to_file(tape1, tape_file1);
  //save_tape_to_file(tape2, tape_file2);
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

void print_out_file(const char name[])
{
  FILE *file;
  char *f_name = (char *)calloc(FIRST_NAME_LEN + 1, sizeof(char));
  char *l_name = (char *)calloc(LAST_NAME_LEN + 1, sizeof(char));

  file = fopen(name, "r");
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

void print_records(Record *records, int num) {
  for (int i = 0; i < num; i++) {
    printf("%.16s, %.16s\n", records[i].last_name, records[i].first_name);
  }
}

void swap_records(Record *rec1, Record *rec2) {
  Record tmp;
  memcpy(&tmp, rec1, sizeof(Record));
  memcpy(rec1, rec2, sizeof(Record));
  memcpy(rec2, &tmp, sizeof(Record));
}

void swap_records(Record_with_index *rec1, Record_with_index *rec2) {
  /*Record tmp;
  int temp = rec1->index;
  rec1->index = rec2->index;
  rec2->index = temp;*/
  Record_with_index tmp;
  memcpy(&tmp, rec1, sizeof(Record_with_index));
  memcpy(rec1, rec2, sizeof(Record_with_index));
  memcpy(rec2, &tmp, sizeof(Record_with_index));
}

void heapify(Record *records, int size, int root_index) {
  int heap_top_index = root_index;
  int left_child_index = 2 * root_index + 1;
  int right_child_index = 2 * root_index + 2;

  if (left_child_index < size) {
    if (compare_records(&records[left_child_index], &records[heap_top_index]) > 0) {
      heap_top_index = left_child_index;
    }
  }

  if (right_child_index < size) {
    if (compare_records(&records[right_child_index], &records[heap_top_index]) > 0) {
      heap_top_index = right_child_index;
    }
  }

  if (heap_top_index != root_index) {
    swap_records(&records[root_index], &records[heap_top_index]);
    heapify(records, size, heap_top_index);
  }
}

void heapify(Record_with_index *records, int size, int root_index) {
  int heap_top_index = root_index;
  int left_child_index = 2 * root_index + 1;
  int right_child_index = 2 * root_index + 2;

  if (left_child_index < size) {
    if (compare_records((&records[left_child_index])->record, (&records[heap_top_index])->record) < 0) {
      heap_top_index = left_child_index;
    }
  }

  if (right_child_index < size) {
    if (compare_records((&records[right_child_index])->record, (&records[heap_top_index])->record) < 0) {
      heap_top_index = right_child_index;
    }
  }

  if (heap_top_index != root_index) {
    swap_records(&records[root_index], &records[heap_top_index]);
    heapify(records, size, heap_top_index);
  }
}

void heap_insert(Record_with_index *records, int size, int index) {
  if (size == 1 || index == 0) {
    return;
  }
  int parent_index = (index - 1) / 2;

  if (compare_records((&records[index])->record, (&records[parent_index])->record) < 0) {
    /*int temp = records[index]->index;
    records[index]->index = records[parent_index]->index;
    records[parent_index]->index = temp;*/
    swap_records(&records[parent_index], &records[index]);
    //swap_records (&records[parent_index]->record, &records[index]->record);
    heapify(records, size, parent_index);
  }
}

void heap_sort_records(Record *records, int size) {
  for (int i = size / 2 - 1; i >= 0; i--) {
    heapify(records, size, i);
  }

  for (int i = size - 1; i > 0; i--) {
    swap_records (&records[0], &records[i]);
    heapify (records, i, 0);
  }

  /*for (int i = 0; i < size - 1; i++) {
    heapify(&records[i], size - i, 0);
  }*/
}

void save_run(Record *run, int size, int n) {
  FILE *file;

  char name[] = "run000";
  name[3] = '0' + n / 100;
  name[4] = '0' + (n % 100) / 10;
  name[5] = '0' + n % 10;

  file = fopen(name, "w");

  fwrite(run, sizeof(Record), size, file);
  disk_saves += size / RECORDS_IN_PAGE;
  if (size % RECORDS_IN_PAGE) {
    disk_saves++;
  }

  fclose(file);
}

void load_file(const char name[]) {
  FILE *file;
  Record records[MAX_RUN];
  int runs = 0;

  file = fopen(name, "r");

  while (1) {
    int read_records;

    read_records = fread(records, sizeof(Record), MAX_RUN, file);
    if (!read_records) {
      break;
    }
    disk_reads += read_records / RECORDS_IN_PAGE;
    if (read_records % RECORDS_IN_PAGE) {
      disk_reads++;
    }

    heap_sort_records(records, read_records);
    //print_records(records, read_records);
    save_run(records, read_records, runs);
    runs++;
    if(runs == 999) {
      printf ("Too many runs!\n");
      return;
    }
  }
  int sorts = 0;
  do {
    runs = merge_runs(runs);
    sorts++;
    if (runs){
      printf("Po %d fazie sortowania. Wyświetlić taśmy? [Y/n]", sorts);
      while((getchar())!='\n');
      char c = getchar();
      if (c == 'Y' || c == 'y') {
        for (int i = 0; i < runs; i++) {
          char name[] = "run000";
          name[3] = '0' + i / 100;
          name[4] = '0' + (i % 100) / 10;
          name[5] = '0' + i % 10;
          print_out_file(name);
        }
      }
    }
  } while (runs);

  fclose(file);
}

// K-way merge (min-heap)
int merge_runs(int run_count) {
  FILE *in_files[BUFFERS_AVAILABLE - 1];
  FILE *out_file;
  Record input_records[RECORDS_IN_PAGE * (BUFFERS_AVAILABLE - 1)];
  //Record output_records[BUFFERS_AVAILABLE];
  int records_left_in_run[BUFFERS_AVAILABLE - 1];
  int current_run_index[BUFFERS_AVAILABLE - 1];
  int runs_left = run_count;
  int runs_loaded = 0;
  int merge_count = 0;
  run_count = 0;
  Record inf_record;
  inf_record.last_name[0] = 0xff;

  // keep loading runs until none left
  while (runs_left) {
    char out_name[] = "mrg000";
    out_name[3] = '0' + run_count / 100;
    out_name[4] = '0' + (run_count % 100) / 10;
    out_name[5] = '0' + run_count % 10;

    run_count++;

    if (runs_left == 1) {
      char run_name[] = "run000";
      int run_num = merge_count * (BUFFERS_AVAILABLE - 1);
      run_name[3] = '0' + run_num / 100;
      run_name[4] = '0' + (run_num % 100) / 10;
      run_name[5] = '0' + run_num % 10;
      remove(out_name);
      rename(run_name, out_name);
      merge_count++;
      break;
    }

    out_file = fopen(out_name, "w");

    if (runs_left > (BUFFERS_AVAILABLE - 1)) {
      runs_loaded = BUFFERS_AVAILABLE - 1;
    }
    else {
      runs_loaded = runs_left;
    }


    for (int i = 0; i < (BUFFERS_AVAILABLE - 1), i < runs_loaded; i++) {
      int run_num = merge_count * (BUFFERS_AVAILABLE - 1) + i;
      char name[] = "run000";
      name[3] = '0' + run_num / 100;
      name[4] = '0' + (run_num % 100) / 10;
      name[5] = '0' + run_num % 10;
      in_files[i] = fopen(name, "r");
    }

    int records_loaded = 0;
    bool records_left;
    int heap_size = 0;

    do {
      Record_with_index output_records[BUFFERS_AVAILABLE - 1];
      
      records_left = false;
      // keep loading page of records from each run until no more left
      for (int i = 0; i < runs_loaded; i++) {

        records_loaded = fread(&input_records[i * RECORDS_IN_PAGE], sizeof(Record), RECORDS_IN_PAGE, in_files[i]);  // part of run loaded
        records_left_in_run[i] = records_loaded;  // records loaded from run
        current_run_index[i] = 0;
        if (records_loaded) {
          disk_reads++;
        }
      }

      for (int i = 0; i < runs_loaded; i++) {
        output_records[i].record = &input_records[i * RECORDS_IN_PAGE];
        output_records[i].index = i;
        heap_size++;
        heap_insert(output_records, heap_size, i);
      }

      Record ordered_records[RECORDS_IN_PAGE];
      int ordered_records_index = 0;

      while (heap_size) {
        //fwrite(output_records->record, sizeof(Record), 1, out_file);

        memcpy(&ordered_records[ordered_records_index], output_records->record, sizeof(Record));
        ordered_records_index++;

        current_run_index[output_records->index]++;
        records_left_in_run[output_records->index]--;
        if (records_left_in_run[output_records->index] > 0) {
          output_records[0].record++;
          heapify(output_records, heap_size, 0);
        } else {
          // run out of records in run, load more
          int i = output_records->index;
          records_loaded = fread(&input_records[i * RECORDS_IN_PAGE], sizeof(Record), RECORDS_IN_PAGE, in_files[i]);  // part of run loaded
          records_left_in_run[i] = records_loaded;  // records loaded from run
          current_run_index[i] = 0;
          if (records_loaded) {
            disk_reads++;
          }

          if (records_left_in_run[output_records->index] > 0) {
            output_records[0].record = &input_records[i * RECORDS_IN_PAGE];
            heapify(output_records, heap_size, 0);
          } else {
            heap_size--;
            swap_records(&output_records[0], &output_records[heap_size]);
            heapify(output_records, heap_size, 0);
          }
        }

        if (ordered_records_index == RECORDS_IN_PAGE || heap_size == 0) {
          fwrite(ordered_records, sizeof(Record), ordered_records_index, out_file);
          printf("records saved: %d\n", ordered_records_index);
          ordered_records_index = 0;
          disk_saves++;
        }
      }

    } while (heap_size);

    for (int i = 0; i < runs_loaded; i++) {
      fclose(in_files[i]);
      int run_num = merge_count * (BUFFERS_AVAILABLE - 1) + i;
      char name[] = "run000";
      name[3] = '0' + run_num / 100;
      name[4] = '0' + (run_num % 100) / 10;
      name[5] = '0' + run_num % 10;
      remove(name);
    }
    fclose(out_file);
    runs_left = runs_left - runs_loaded;
    merge_count++;
  }

  if (merge_count > 1) {
    printf("merge count: %d\n", merge_count);
    for (int i = 0; i < merge_count; i++) {
      char merge_name[] = "mrg000";
      merge_name[3] = '0' + i / 100;
      merge_name[4] = '0' + (i % 100) / 10;
      merge_name[5] = '0' + i % 10;
      char run_name[] = "run000";
      run_name[3] = '0' + i / 100;
      run_name[4] = '0' + (i % 100) / 10;
      run_name[5] = '0' + i % 10;

      remove(run_name);
      rename(merge_name, run_name);
    }
    return merge_count;
  }
  else {
    for(int i = 0; i < runs_loaded; i++) {
      char run_name[] = "run000";
      run_name[3] = '0' + i / 100;
      run_name[4] = '0' + (i % 100) / 10;
      run_name[5] = '0' + i % 10;

      remove(run_name);
    }
    remove("sorted.data");
    rename("mrg000", "sorted.data");
    return 0;
  }
}

void save_tape_to_file(Tape *tape, const char name[]) {
  FILE *file;
  file = fopen(name, "w");
  char *string = (char *)calloc(FIRST_NAME_LEN + 1, sizeof(char));
  string[FIRST_NAME_LEN] = '\0';

  Run *current_run = tape->first;
  Value *current_value = current_run->first;

  while (current_run != NULL) {
    current_value = current_run->first;
    while (current_value != NULL) {
      memset(string, 0, FIRST_NAME_LEN);
      strcpy(string, current_value->last_name);
      fwrite(string, sizeof(char), LAST_NAME_LEN, file);
      memset(string, 0, FIRST_NAME_LEN);
      strcpy(string, current_value->first_name);
      fwrite(string, sizeof(char), FIRST_NAME_LEN, file);
      current_value = current_value->next;
    }
    current_run = current_run->next;
  }

  fclose(file);
}


int main()
{
  char name[MAX_FILE_NAME_LEN];
  disk_reads = 0;
  disk_saves = 0;
  printf("Podaj nazwe pliku: \n");
  scanf("%s", name);
  load_file("names.data");
  //print_out_file("sorted.data");

  printf("disk reads: %d\n", disk_reads);
  printf("disk saves: %d\n", disk_saves);

  return 0;
}
