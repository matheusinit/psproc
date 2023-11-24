#include "file.h"
#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

char **split_string_by_delimiter(char *string, int *size, char *delimiter) {
  int capacity = 52;
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

char *get_path_for_process(char *process_id) {
  char *processes_folder = "/proc/";

  size_t pid_path_length = strlen(processes_folder) + strlen(process_id);

  char *pid_path = malloc(pid_path_length + 1);

  strcpy(pid_path, processes_folder);
  strcat(pid_path, process_id);

  return pid_path;
}

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

  for (int i = 0; i >= 0 && (size_t)i < strlen(string); i++) {

    if (isalpha(string[i]) && !isdigit(string[i])) {
      has_letters = 1;
      break;
    }
  }

  return has_letters;
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

int main() {
  struct dirent *files;
  int array_capacity = 1000;
  int processes_id_size = 0;
  char **processes_id = malloc(sizeof(char *) * array_capacity);

  DIR *proc_dir = check_and_open_directory("/proc");

  processes_id = get_processes_ids(proc_dir, &processes_id_size);

  printf("%s\t%s\t%s\n", "PID", "STATE", "COMMAND");

  for (int index = 0; index < processes_id_size; index++) {
    char *selected_pid = processes_id[index];

    char *pid_path = get_path_for_process(selected_pid);

    DIR *pid_dir = check_and_open_directory(pid_path);

    char *pid, *state, *command;

    while ((files = readdir(pid_dir))) {

      char **file_content_array;

      if (strcmp(files->d_name, ".") == 0) {
        continue;
      }

      if (strcmp(files->d_name, "..") == 0) {
        continue;
      }

      if (strcmp(files->d_name, "stat") == 0) {

        int file_content_array_size = 0;
        char *file_content = get_file_content(files->d_name, pid_path);

        file_content_array = split_string_by_delimiter(
            file_content, &file_content_array_size, " ");

        pid = file_content_array[0];
        state = file_content_array[2];
      }

      if (strcmp(files->d_name, "cmdline") == 0) {
        char *file_content = get_file_content(files->d_name, pid_path);

        command = file_content;

        if (strcmp(command, "") == 0) {
          command = file_content_array[1];
        }
      }
    }

    free(pid_path);
    closedir(pid_dir);

    printf("%s\t%s\t%s\n", pid, state, command);
  }

  free(processes_id);
  closedir(proc_dir);

  return EXIT_SUCCESS;
}
