#include "process.h"

#include "file.h"
#include "utils.h"
#include <ctype.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char *get_path_for_process(char *process_id) {
  char *processes_folder = "/proc/";

  size_t pid_path_length = strlen(processes_folder) + strlen(process_id);

  char *pid_path = malloc(pid_path_length + 1);

  strcpy(pid_path, processes_folder);
  strcat(pid_path, process_id);

  return pid_path;
}

char **get_processes_ids(DIR *directory, int *array_size) {
  struct dirent *files;
  int required_size = 1000;
  char **array = malloc(sizeof(char *) * required_size);
  int array_index = 0;

  while ((files = readdir(directory))) {
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

    array[array_index] = directory_name;
    array_index++;
    (*array_size)++;
  }

  return array;
}

int get_clock_ticks_by_pid(char *pid) {
  char *pid_path = get_path_for_process(pid);
  char *file = get_file_content("/stat", pid_path);
  int size = 0;

  char **array = split_string_by_delimiter(file, &size, " ");

  int user_time = atoi(array[13]);
  int system_time = atoi(array[14]);

  int total_time = user_time + system_time;

  free(pid_path);
  free(file);
  free(array);

  return total_time;
}

int get_total_clock_ticks() {
  char *file = get_file_content("stat", "/proc");

  int capacity2 = 1024 * 8;
  int file_separated_by_lines_size = 0;
  char **file_separated_by_lines = calloc(capacity2 + 1, sizeof(char *));

  char *token2 = strtok(file, "\n");

  while (token2 != NULL) {
    int token_length = strlen(token2);
    file_separated_by_lines[file_separated_by_lines_size] =
        calloc(token_length + 1, sizeof(char));
    strcpy(file_separated_by_lines[file_separated_by_lines_size], token2);
    file_separated_by_lines_size++;

    token2 = strtok(NULL, "\n");
  }

  // int size = 0;
  //
  // int capacity = 1024;
  // char **array = calloc(capacity + 1, sizeof(char *));
  //
  // char *token = strtok(file, " ");
  //
  // while (token != NULL) {
  //   int token_length = strlen(token);
  //   array[size] = calloc(token_length + 1, sizeof(char));
  //   strcpy(array[size], token);
  //   printf("Token: %s\n", token);
  //   printf("Token in Array: %s\n", array[size]);
  //
  //   size++;
  //   token = strtok(NULL, " ");
  // }

  // printf("%s\n", token);
  // char **array = split_string_by_delimiter(file, &size, " ");

  return 0;
}

int calculate_cpu_usage(char *pid) {
  int total_time_before = get_clock_ticks_by_pid(pid);

  printf("%d\n", total_time_before);

  sleep(1);

  int total_time_after = get_clock_ticks_by_pid(pid);

  printf("%d\n", total_time_after);

  get_total_clock_ticks();

  return 0;
}
