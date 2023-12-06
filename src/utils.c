#include "utils.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int has_letters_in_string(char *string) {
  int has_letters = 0;

  for (int i = 0; i >= 0 && (size_t)i < strlen(string); i++) {

    if (isalpha(string[i]) && !isdigit(string[i])) {
      has_letters = 1;
      break;
    }
  }

  return has_letters;
}

char **split_string_by_delimiter(char *string, int *size, char *delimiter,
                                 int capacity) {
  char **array = calloc(capacity + 1, sizeof(char *));

  char *token = strtok(string, delimiter);

  while (token != NULL) {
    int token_length = strlen(token);
    array[*size] = calloc(token_length + 1, sizeof(char));
    strcpy(array[*size], token);
    (*size)++;

    token = strtok(NULL, " ");
  }

  return array;
}

void print_directory_children_from_array(char **array, int array_size) {
  for (int i = 0; i < array_size; i++) {
    if (array[i] == NULL) {
      break;
    }

    printf("%s\n", array[i]);
  }
}
