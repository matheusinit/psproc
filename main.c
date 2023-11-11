#include <ctype.h>
#include <dirent.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_directory_children_from_array(char **array, int array_size) {
  for (int i = 0; i < array_size; i++) {
    if (array[i] == NULL) {
      break;
    }

    printf("%s\n", array[i]);
  }
}

int has_letters_in_string(char *string) {
  int has_letters = 0;

  for (char i = 0; i < strlen(string); i++) {

    if (isalpha(string[i]) && !isdigit(string[i])) {
      has_letters = 1;
      break;
    }
  }

  return has_letters;
}

int main() {
  DIR *dir;
  struct dirent *files;
  char *list[1000];
  int list_index = 0;

  dir = opendir("/proc");

  if (dir == NULL) {
    printf("Error opening directory\n");
    exit(1);
  }

  while ((files = readdir(dir))) {
    char *directory_name = files->d_name;

    if (strcmp(directory_name, ".") == 0) {
      continue;
    }

    if (strcmp(directory_name, "..") == 0) {
      continue;
    }

    int has_letters = has_letters_in_string(directory_name);

    if (has_letters == 1) {
      continue;
    }

    list[list_index] = directory_name;
    list_index++;
  }

  int array_size = sizeof(list);

  print_directory_children_from_array(list, array_size);

  return EXIT_SUCCESS;
}
