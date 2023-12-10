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

  char **array = split_string_by_delimiter(file, &size, " ", 52);

  int user_time = atoi(array[13]);
  int system_time = atoi(array[14]);

  int total_time = user_time + system_time;

  return total_time;
}

char **separate_file_by_lines(char *file) {
  int capacity2 = 1024 * 8;
  int size = 0;
  char **list = calloc(capacity2 + 1, sizeof(char *));

  char *token = strtok(file, "\n");

  while (token != NULL) {
    int token_length = strlen(token);
    list[size] = calloc(token_length + 1, sizeof(char));
    strcpy(list[size], token);
    size++;

    token = strtok(NULL, "\n");
  }

  return list;
}

int get_total_clock_ticks() {
  char *file = get_file_content("stat", "/proc");

  char **file_separated_by_lines = separate_file_by_lines(file);

  char *cpu_ticks = file_separated_by_lines[0];

  int size = 0;

  char **array = split_string_by_delimiter(cpu_ticks, &size, " ", 52);

  int total_clock_ticks = 0;

  for (int i = 2; i < size - 5; i++) {
    total_clock_ticks += atoi(array[i]);
  }

  return total_clock_ticks;
}

float *calculate_cpu_usage(char **pid_list, int size) {
  int *total_time_before = calloc(size + 1, sizeof(int));
  int *total_clock_ticks_before = calloc(size + 1, sizeof(int));
  int *total_time_after = calloc(size + 1, sizeof(int));
  int *total_clock_ticks_after = calloc(size + 1, sizeof(int));

  for (int i = 0; i < size; i++) {
    char *pid = pid_list[i];

    total_time_before[i] = get_clock_ticks_by_pid(pid);
    total_clock_ticks_before[i] = get_total_clock_ticks();
  }

  sleep(1);

  for (int i = 0; i < size; i++) {
    char *pid = pid_list[i];

    total_time_after[i] = get_clock_ticks_by_pid(pid);
    total_clock_ticks_after[i] = get_total_clock_ticks();
  }

  float *cpu_usage_list = calloc(size, sizeof(int));

  for (int i = 0; i < size; i++) {
    int pid_total_time = total_time_after[i] - total_time_before[i];
    int cpu_usage_in_second =
        total_clock_ticks_after[i] - total_clock_ticks_before[i];

    float cpu_usage = 4 * pid_total_time * 100 / (float)(cpu_usage_in_second);

    cpu_usage_list[i] = cpu_usage;
  }

  return cpu_usage_list;
}
