#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define FIRST_NAME_LEN 16
#define LAST_NAME_LEN 16
#define DEFAULT_NAME_COUNT 25

int generate_number(int min, int max) {
  int delta = max - min + 1;

  return rand()%delta + min;
}

char generate_char() {
  char c = (char)generate_number((int)'a', (int)'z');

  return c;
}

void generate_name(char **f_name, char **l_name) {
  int fnlen, lnlen;

  fnlen = generate_number(3, FIRST_NAME_LEN);
  lnlen = generate_number(5, LAST_NAME_LEN);

  for(int i = 0; i < fnlen; i++){
    (*f_name)[i] = generate_char();
  }
  (*f_name)[fnlen] = '\0';

  for(int i = 0; i < lnlen; i++){
    (*l_name)[i] = generate_char();
  }
  (*l_name)[lnlen] = '\0';
}

int main(int argc, char *argv[]) {
  char *first_name = (char *)calloc(FIRST_NAME_LEN + 1, sizeof(char));
  char *last_name = (char *)calloc(LAST_NAME_LEN + 1, sizeof(char));
  int seed;
  time_t t;
  FILE *file;

  first_name[FIRST_NAME_LEN] = '\0';
  last_name[LAST_NAME_LEN] = '\0';

  file = fopen("names.data", "w");
  
  seed = time(&t);
  srand(seed);

  for(int i = 0; i < DEFAULT_NAME_COUNT; i++) {
    generate_name(&first_name, &last_name);
    fwrite(last_name, sizeof(char), LAST_NAME_LEN, file);
    fwrite(first_name, sizeof(char), FIRST_NAME_LEN, file);
    printf("%s %s\n", last_name, first_name);
  }

  fclose(file);
  
  if(argc == 2) {
    printf("Generating %d names\n", strtol(argv[1], NULL, 10));
  } else {
    printf("Generating %d names\n", DEFAULT_NAME_COUNT);
  }
  return 0;
}