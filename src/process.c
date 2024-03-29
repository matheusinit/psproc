#include "process.h"
#include "file.h"
#include "utils.h"

#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

char **separate_file_by_lines(char *file);
int get_clock_ticks_by_pid(char *pid);
int get_total_clock_ticks();
float get_memory_usage_by_pid(float rss);
char *get_process_owner_by_pid_path(char *pid_path);
char **get_files_array_by_pid_dir(DIR *pid_dir);
char *get_pid(char **stat_file_content_array);
char *get_state(char **stat_file_content_array);
int get_rss(char **stat_file_content_array);
char *get_command(char *pid_path);

void print_processes_info(struct process **processes, int size) {
  for (int index = 0; index < size; index++) {
    struct process *current_process = processes[index];
    int column_width = 8;

    printf("%-*s %-*s %-*s %-*.2f %-*.2f %-*.0f %-*s\n", column_width * 2,
           current_process->user, column_width, current_process->pid,
           column_width, current_process->state, column_width * 2,
           current_process->cpu_usage, column_width * 2,
           current_process->memory_usage, column_width, current_process->rss,
           column_width, current_process->command);
  }
}

struct process **iterate_processes_and_get_process_info(char **processes_id,
                                                        int size,
                                                        float *cpu_usage_list) {
  struct process **list = malloc(sizeof(struct process *) * size);

  for (int index = 0; index < size; index++) {
    char *selected_pid = processes_id[index];

    struct process *current_process = get_process_by_pid(selected_pid);
    current_process->cpu_usage = cpu_usage_list[index];
    list[index] = current_process;
  }

  return list;
}

int get_total_memory(DIR *proc_dir) {
  struct dirent *files;

  int total_memory = 0;

  while ((files = readdir(proc_dir))) {
    if (strcmp(files->d_name, ".") == 0) {
      continue;
    }

    if (strcmp(files->d_name, "..") == 0) {
      continue;
    }

    if (strcmp(files->d_name, "meminfo") == 0) {
      char *file_content = get_file_content(files->d_name, "/proc");

      int file_content_array_size = 0;
      char **file_content_array = split_string_by_delimiter(
          file_content, &file_content_array_size, " ", 128);

      char *total_memory_string = file_content_array[1];

      total_memory = atoi(total_memory_string);
    }
  }

  return total_memory;
}

struct process *get_process_by_pid(char *pid) {
  struct process *current_process = malloc(sizeof(struct process));

  current_process->pid_path = "NULL";
  current_process->pid = "NULL";
  current_process->command = "NULL";
  current_process->state = "NULL";
  current_process->user = "NULL";

  char *pid_path = get_path_for_process(pid);

  current_process->pid_path = pid_path;

  if (strcmp(current_process->pid_path, "NULL") == 0) {
    return current_process;
  }

  DIR *pid_dir = check_and_open_directory(current_process->pid_path);

  char *stat_file_content = get_file_content("stat", pid_path);

  int file_content_array_size = 0;
  char **stat_file_content_array = split_string_by_delimiter(
      stat_file_content, &file_content_array_size, " ", 52);

  char *pid_from_stat = get_pid(stat_file_content_array);

  current_process->pid = pid_from_stat;
  current_process->state = get_state(stat_file_content_array);
  current_process->rss = get_rss(stat_file_content_array);
  current_process->memory_usage = get_memory_usage_by_pid(current_process->rss);
  current_process->command = get_command(pid_path);

  char *user = get_process_owner_by_pid_path(pid_path);
  current_process->user = user;

  free(pid_path);

  closedir(pid_dir);

  return current_process;
}

char *get_pid(char **stat_file_content_array) {
  return stat_file_content_array[0];
}

char *get_state(char **stat_file_content_array) {
  return stat_file_content_array[2];
}

int get_rss(char **stat_file_content_array) {
  int rss_index = 23;
  return atoi(stat_file_content_array[rss_index]) * 4;
}

char *get_command(char *pid_path) {
  char *command = get_file_content("cmdline", pid_path);

  if (strcmp(command, "") == 0) {
    char *file_content = get_file_content("stat", pid_path);

    int file_content_array_size = 0;

    char **file_content_array = split_string_by_delimiter(
        file_content, &file_content_array_size, " ", 52);

    char *process_name = file_content_array[1];

    free(file_content);
    free(file_content_array);

    return process_name;
  }

  return command;
}

char **get_files_array_by_pid_dir(DIR *pid_dir) {
  struct dirent *files;

  char **files_array = malloc(sizeof(char *) * 512);
  int last_item_index = 0;

  while ((files = readdir(pid_dir))) {
    if (strcmp(files->d_name, ".") == 0) {
      continue;
    }

    if (strcmp(files->d_name, "..") == 0) {
      continue;
    }

    files_array[last_item_index] = files->d_name;
    last_item_index++;
  }

  return files_array;
}

char *get_process_owner_by_pid_path(char *pid_path) {
  struct stat stat_buffer;

  if (lstat(pid_path, &stat_buffer) == -1) {
    perror("lstat");
    exit(EXIT_FAILURE);
  }

  struct passwd *pwd = getpwuid(stat_buffer.st_uid);
  char *user = malloc(sizeof(char) * 52);

  if (pwd != NULL) {
    strcpy(user, pwd->pw_name);
  } else {
    printf("error: user with uid %d not found\n", stat_buffer.st_uid);
  }

  return user;
}

float get_memory_usage_by_pid(float rss) {

  DIR *proc_dir = check_and_open_directory("/proc");

  int total_memory = get_total_memory(proc_dir);

  closedir(proc_dir);

  float percentage = (rss * 100) / total_memory;

  return percentage;
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

float *calculate_cpu_usage(char **pid_list, int size) {
  int *total_time_before = calloc(size + 1, sizeof(int));
  int *total_clock_ticks_before = calloc(size + 1, sizeof(int));
  int *total_time_after = calloc(size + 1, sizeof(int));
  int *total_clock_ticks_after = calloc(size + 1, sizeof(int));
  float *cpu_usage_list = calloc(size, sizeof(int));

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

  for (int i = 0; i < size; i++) {
    int pid_total_time = total_time_after[i] - total_time_before[i];
    int cpu_usage_in_second =
        total_clock_ticks_after[i] - total_clock_ticks_before[i];

    float cpu_usage = 4 * pid_total_time * 100 / (float)(cpu_usage_in_second);

    cpu_usage_list[i] = cpu_usage;
  }

  free(total_time_before);
  free(total_clock_ticks_before);
  free(total_time_after);
  free(total_clock_ticks_after);

  return cpu_usage_list;
}

int get_clock_ticks_by_pid(char *pid) {
  char *pid_path = get_path_for_process(pid);
  char *file = get_file_content("/stat", pid_path);
  free(pid_path);
  int size = 0;

  char **array = split_string_by_delimiter(file, &size, " ", 52);
  free(file);

  int user_time = atoi(array[13]);
  int system_time = atoi(array[14]);

  int total_time = user_time + system_time;

  return total_time;
}

char *get_path_for_process(char *process_id) {
  char *processes_folder = "/proc/";

  size_t pid_path_length = strlen(processes_folder) + strlen(process_id);

  char *pid_path = malloc(pid_path_length + 1);

  strcpy(pid_path, processes_folder);
  strcat(pid_path, process_id);

  return pid_path;
}

int get_total_clock_ticks() {
  char *file = get_file_content("stat", "/proc");

  char **file_separated_by_lines = separate_file_by_lines(file);

  free(file);

  char *cpu_ticks = file_separated_by_lines[0];

  free(file_separated_by_lines);

  int size = 0;

  char **array = split_string_by_delimiter(cpu_ticks, &size, " ", 52);

  int total_clock_ticks = 0;

  for (int i = 2; i < size - 5; i++) {
    total_clock_ticks += atoi(array[i]);
  }

  return total_clock_ticks;
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
