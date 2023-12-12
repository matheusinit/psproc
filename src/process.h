#ifndef PROCESS_H
#define PROCESS_H

#include <dirent.h>

struct process {
  char *pid;
  char *state;
  char *command;
  char *pid_path;
  float cpu_usage;
};

struct process *get_process_by_pid(char *pid);
char **get_processes_ids(DIR *directory, int *array_size);
char *get_path_for_process(char *process_id);
float *calculate_cpu_usage(char **pid_list, int size);

#endif
