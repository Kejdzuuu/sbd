#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define FIRST_NAME_LEN 16
#define LAST_NAME_LEN FIRST_NAME_LEN
#define MAX_FILE_NAME_LEN 128
#define PAGE_SIZE 4096
#define RECORDS_IN_PAGE 128
#define BUFFERS_AVAILABLE 16
#define MAX_RUN (RECORDS_IN_PAGE * BUFFERS_AVAILABLE)

char first_names[][FIRST_NAME_LEN] = {
  "james", "mary", "john", "patricia", "robert", "jennifer",
  "michael", "linda", "william", "elizabeth", "david", "barbara"
};

char last_names[][LAST_NAME_LEN] = {
  "smith", "johnson", "williams", "jones", "brown", "davis",
  "miller", "wilson", "moore", "taylor", "anderson", "thomas"
};

#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))
#define FIRST_NAMES_SIZE NELEMS(first_names)
#define LAST_NAMES_SIZE NELEMS(last_names)


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


void sort_file(const char name[]);

void print_out_file(const char name[]);

int merge_runs(int runs, const char name[]);

char get_user_input();

int disk_reads;
int disk_saves;
int records_read;
int records_saved;