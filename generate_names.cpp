#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define FIRST_NAME_LEN 16
#define LAST_NAME_LEN 16
#define DEFAULT_NAME_COUNT 25

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

int main(int argc, char *argv[]) {
  char *first_name = (char *)calloc(FIRST_NAME_LEN + 1, sizeof(char));
  char *last_name = (char *)calloc(LAST_NAME_LEN + 1, sizeof(char));
  int seed;
  time_t t;
  int number_of_names;
  FILE *file;

  if(argc == 2) {
    number_of_names = strtol(argv[1], NULL, 10);
  } else {
    number_of_names = DEFAULT_NAME_COUNT;
  }
  printf("Generating %d names\n", number_of_names);

  first_name[FIRST_NAME_LEN] = '\0';
  last_name[LAST_NAME_LEN] = '\0';

  file = fopen("names.data", "w");
  
  seed = time(&t);
  srand(seed);

  for(int i = 0; i < number_of_names; i++) {
    generate_name(first_name, last_name);
    fwrite(last_name, sizeof(char), LAST_NAME_LEN, file);
    fwrite(first_name, sizeof(char), FIRST_NAME_LEN, file);
    printf("%s %s\n", last_name, first_name);
  }

  fclose(file);
  return 0;
}