#include <ctype.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

  int list_size = sizeof(list);
  for (int i = 0; i < list_size; i++) {
    if (list[i] == NULL) {
      break;
    }

    printf("%s\n", list[i]);
  }

  return EXIT_SUCCESS;
}
