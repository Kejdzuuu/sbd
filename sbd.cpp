#include "sbd.h"

int generate_number(int min, int max) {
  int delta = max - min;
  return rand()%delta + min;
}

char generate_char() {
  char c = (char)generate_number((int)'a', (int)'z');
  return c;
}

void generate_name(char *f_name, char *l_name) {
  int f_name_index, l_name_index;

  f_name_index = generate_number(0, FIRST_NAMES_SIZE);
  l_name_index = generate_number(0, LAST_NAMES_SIZE);

  memcpy (f_name, first_names[f_name_index], FIRST_NAMES_SIZE);
  memcpy (l_name, last_names[l_name_index], LAST_NAMES_SIZE);
}

void generate_random_file(char *file_name) {
  char *first_name = (char *)calloc(FIRST_NAME_LEN + 1, sizeof(char));
  char *last_name = (char *)calloc(LAST_NAME_LEN + 1, sizeof(char));
  int seed;
  time_t t;
  FILE *file;
  int number_of_names;
  
  printf ("How many records?\n");
  int result = scanf ("%d", &number_of_names);
  if (result == EOF) {
    return;
  }

  printf("Generating %d names\n", number_of_names);

  first_name[FIRST_NAME_LEN] = '\0';
  last_name[LAST_NAME_LEN] = '\0';

  file = fopen(file_name, "w");
  
  seed = time(&t);
  srand(seed);

  for(int i = 0; i < number_of_names; i++) {
    generate_name(first_name, last_name);
    fwrite(last_name, sizeof(char), LAST_NAME_LEN, file);
    fwrite(first_name, sizeof(char), FIRST_NAME_LEN, file);
  }

  fclose(file);
  free(first_name);
  free(last_name);
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

void print_out_file(const char name[])
{
  FILE *file;
  char *f_name = (char *)calloc(FIRST_NAME_LEN + 1, sizeof(char));
  char *l_name = (char *)calloc(LAST_NAME_LEN + 1, sizeof(char));

  printf ("Printing %s:\n", name);

  file = fopen(name, "r");
  f_name[FIRST_NAME_LEN] = '\0';
  l_name[LAST_NAME_LEN] = '\0';

  while (1) {
    int read_bytes;

    read_bytes = fread (l_name, sizeof(char), LAST_NAME_LEN, file);
    if (!read_bytes) {
      break;
    }

    read_bytes = fread (f_name, sizeof(char), FIRST_NAME_LEN, file);
    if (!read_bytes) {
      break;
    }

    printf ("%s %s\n", l_name, f_name);
  }

  free (f_name);
  free (l_name);
  fclose (file);
  printf ("\n");
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
    swap_records(&records[parent_index], &records[index]);
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

void sort_file(const char name[]) {
  FILE *file;
  Record records[MAX_RUN];
  int runs = 0;
  int sorts = 0;
  char c;

  disk_reads = 0;
  disk_saves = 0;

  printf ("Print file? [Y/n] ");
  c = get_user_input ();
  if (c == 'Y' || c == 'y') {
    print_out_file (name);
    printf ("Press any key...\n");
    get_user_input ();
  }

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
      printf ("Press any key...\n");
      get_user_input ();
      return;
    }
  }
  
  fclose(file);

  do {
    runs = merge_runs(runs, name);
    sorts++;
    if (runs) {
      printf("%d sort cycles done. Print tapes? [Y/n]", sorts);
      c = get_user_input ();
      if (c == 'Y' || c == 'y') {
        for (int i = 0; i < runs; i++) {
          char run_name[] = "run000";
          run_name[3] = '0' + i / 100;
          run_name[4] = '0' + (i % 100) / 10;
          run_name[5] = '0' + i % 10;
          print_out_file (run_name);
        }
        printf ("Press any key...\n");
        get_user_input ();
      }
    }
  } while (runs);

  printf("Print sorted file? [Y/n]");
  c = get_user_input ();
  if (c == 'Y' || c == 'y') {
    print_out_file (name);
  }

  printf("Sort cycles: %d\n", sorts);
  printf("Pages read: %d\n", disk_reads);
  printf("Pages saved: %d\n", disk_saves);
  printf ("Press any key...\n");
  get_user_input ();
}

// K-way merge (min-heap)
int merge_runs(int run_count, const char file_name[]) {
  FILE *in_files[BUFFERS_AVAILABLE - 1];
  FILE *out_file;
  Record input_records[RECORDS_IN_PAGE * (BUFFERS_AVAILABLE - 1)];
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
        ordered_records_index = 0;
        disk_saves++;
      }
    }

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
    remove(file_name);
    rename("mrg000", file_name);
    return 0;
  }
}

char get_user_input() {
  char c = getchar ();
  if (c == '\n')
    return getchar();
  return c;
}

void take_user_input (char *name) {
  char *first_name = (char *)calloc(FIRST_NAME_LEN, sizeof(char));
  char *last_name = (char *)calloc(LAST_NAME_LEN, sizeof(char));
  int number_of_names;
  FILE *file;

  printf ("How many records?\n");
  int result = scanf ("%d", &number_of_names);
  if (result == EOF) {
    return;
  }

  file = fopen (name, "w");

  for (int i = 0; i < number_of_names; i++) {
    memset (last_name, 0, LAST_NAME_LEN);
    memset (first_name, 0, FIRST_NAME_LEN);
    printf ("Last name: ");
    scanf("%s", last_name);
    printf ("First name: ");
    scanf("%s", first_name);
    fwrite(last_name, sizeof(char), LAST_NAME_LEN, file);
    fwrite(first_name, sizeof(char), FIRST_NAME_LEN, file);
  }

  free (first_name);
  free (last_name);
  fclose (file);
}


int main()
{
  char name[MAX_FILE_NAME_LEN];
  char user_input;

  printf ("Merging with large buffers\n");
  while(1) {
    printf ("[1] Sort a file\n");
    printf ("[2] Generate records\n");
    printf ("[3] Enter records\n");
    printf ("[q] Quit\n");
    user_input = get_user_input ();

    switch (user_input) {
      case '1':
        printf ("File name: ");
        scanf("%s", name);
        sort_file (name);
        break;
      case '2':
        printf ("File name: ");
        scanf("%s", name);
        generate_random_file (name);
        sort_file (name);
        break;
      case '3':
        printf ("File name: ");
        scanf("%s", name);
        take_user_input (name);
        sort_file (name);
        break;
      default:
        return 0;
    }
  }

  return 0;
}
