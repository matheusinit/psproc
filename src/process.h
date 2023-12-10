#ifndef PROCESS_H
#define PROCESS_H

#include <dirent.h>

char **get_processes_ids(DIR *directory, int *array_size);
char *get_path_for_process(char *process_id);
float *calculate_cpu_usage(char **pid_list, int size);

#endif
