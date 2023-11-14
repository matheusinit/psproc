#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

char *get_buffer_from_file(char *file_name, char *dir_path) {
  int file_content_buffer_capacity = sizeof(char) * 1024 * 10;
  char *file_content = malloc(file_content_buffer_capacity);
  int file_content_buffer_size = 0;

  size_t stat_path_length = strlen(dir_path) + strlen(file_name) + 1;

  char *pid_stat_path = malloc(stat_path_length);

  strcpy(pid_stat_path, dir_path);
  strcat(pid_stat_path, "/");
  strcat(pid_stat_path, file_name);

  FILE *exe_file = fopen(pid_stat_path, "r");

  size_t bytes_read;
  char *buffer = malloc(sizeof(char) * 1024);

  if (exe_file == NULL) {
    printf("Error opening file %s\n", file_name);

    exit(1);
  }

  while ((bytes_read = fread(buffer, 1, sizeof(buffer), exe_file)) > 0) {
    strcat(file_content, buffer);
    file_content_buffer_size += bytes_read;

    if (file_content_buffer_size >= file_content_buffer_capacity - 1) {
      file_content_buffer_capacity *= 2;
      file_content = realloc(file_content, file_content_buffer_capacity);
    }
  }

  fclose(exe_file);

  return file_content;
}

char *get_path_for_process(char *process_id) {
  char *processes_folder = "/proc/";

  size_t pid_path_length = strlen(processes_folder) + strlen(process_id);

  char *pid_path = malloc(pid_path_length);

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

  for (char i = 0; i < strlen(string); i++) {

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
  DIR *proc_dir;
  struct dirent *files;
  int array_capacity = 1000;
  int array_size = 0;
  char **array = malloc(sizeof(char *) * array_capacity);
  int array_index = 0;

  proc_dir = opendir("/proc");

  if (proc_dir == NULL) {
    printf("Error opening directory\n");
    exit(1);
  }

  array = get_processes_ids(proc_dir, &array_size);

  char *selected_pid = array[array_size - 1];
  char *pid_path = get_path_for_process(selected_pid);

  DIR *pid_dir = opendir(pid_path);
  printf("%s\n", pid_path);

  if (pid_dir == NULL) {
    printf("Error opening directory of process %s\n", selected_pid);
    exit(1);
  }

  while ((files = readdir(pid_dir))) {
    if (strcmp(files->d_name, ".") == 0) {
      continue;
    }

    if (strcmp(files->d_name, "..") == 0) {
      continue;
    }

    if (strcmp(files->d_name, "stat") == 0) {
      char *file_content = get_buffer_from_file(files->d_name, pid_path);

      printf("%s", file_content);
    }
  }

  return EXIT_SUCCESS;
}
