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
  int size_by_bytes = 0;
  int capacity = sizeof(char) * 64;
  char **array = malloc(capacity);

  char *token = strtok(string, " ");

  while (token != NULL) {
    array[*size] = token;
    (*size)++;
    size_by_bytes += strlen(token);

    token = strtok(NULL, " ");
  }

  return array;
}

char *get_buffer_from_file(char *file_name, char *dir_path) {
  int file_content_buffer_capacity = sizeof(char) * 16;
  char *file_content = malloc(file_content_buffer_capacity);
  int file_content_buffer_size = 0;

  size_t stat_path_length = strlen(dir_path) + strlen(file_name) + 1;

  char *pid_stat_path = malloc(stat_path_length);

  strcpy(pid_stat_path, dir_path);
  strcat(pid_stat_path, "/");
  strcat(pid_stat_path, file_name);

  FILE *exe_file = fopen(pid_stat_path, "r");

  size_t bytes_read;
  int buffer_capacity = sizeof(char) * 16;
  char *buffer = malloc(sizeof(char) * 16);

  if (exe_file == NULL) {
    printf("Error opening file %s\n", file_name);

    exit(EXIT_FAILURE);
  }

  while ((bytes_read = fread(buffer, 1, buffer_capacity, exe_file)) > 0) {
    if (bytes_read < buffer_capacity) {
      buffer[bytes_read] = '\0';
    }

    strcat(file_content, buffer);
    file_content_buffer_size += bytes_read;

    if (file_content_buffer_size >= file_content_buffer_capacity) {
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

DIR *check_and_open_directory(char *path) {
  DIR *directory = opendir(path);

  if (directory == NULL) {
    printf("Error opening directory\n");
    exit(EXIT_FAILURE);
  };

  return directory;
}

int main() {
  struct dirent *files;
  int array_capacity = 1000;
  int processes_id_size = 0;
  char **processes_id = malloc(sizeof(char *) * array_capacity);

  DIR *proc_dir = check_and_open_directory("/proc");

  processes_id = get_processes_ids(proc_dir, &processes_id_size);

  char *selected_pid = processes_id[processes_id_size - 1];
  char *pid_path = get_path_for_process(selected_pid);

  DIR *pid_dir = check_and_open_directory(pid_path);

  char *pid, *state, *command;

  while ((files = readdir(pid_dir))) {
    if (strcmp(files->d_name, ".") == 0) {
      continue;
    }

    if (strcmp(files->d_name, "..") == 0) {
      continue;
    }

    if (strcmp(files->d_name, "stat") == 0) {
      char *file_content = get_buffer_from_file(files->d_name, pid_path);

      char *s = file_content;

      int file_content_array_size = 0;

      char **file_content_array = split_string_by_delimiter(
          file_content, &file_content_array_size, " ");

      pid = file_content_array[0];
      state = file_content_array[2];
    }

    if (strcmp(files->d_name, "cmdline") == 0) {
      char *file_content = get_buffer_from_file(files->d_name, pid_path);

      command = file_content;
    }
  }

  closedir(pid_dir);

  closedir(proc_dir);

  printf("%s\t%s\t%s\n", "PID", "STATE", "COMMAND");
  printf("%s\t%s\t%s\n", pid, state, command);

  return EXIT_SUCCESS;
}
