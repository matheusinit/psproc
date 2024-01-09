#ifndef PROCESS_H
#define PROCESS_H

#include <dirent.h>

struct process {
  char *pid;
  char *state;
  char *command;
  char *pid_path;
  float cpu_usage;
  float rss;
  float memory_usage;
};

struct process *get_process_by_pid(char *pid);
char **get_processes_ids(DIR *directory, int *array_size);
char *get_path_for_process(char *process_id);
float *calculate_cpu_usage(char **pid_list, int size);
struct process **iterate_processes_and_get_process_info(char **processes_id,
                                                        int size,
                                                        float *cpu_usage_list);
void print_processes_info(struct process **processes, int size);

#endif
