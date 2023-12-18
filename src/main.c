#include "file.h"
#include "process.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  int array_capacity = 1000;
  int processes_id_size = 0;
  char **processes_id = malloc(sizeof(char *) * array_capacity);

  DIR *proc_dir = check_and_open_directory("/proc");

  processes_id = get_processes_ids(proc_dir, &processes_id_size);

  float *cpu_usage_list = calculate_cpu_usage(processes_id, processes_id_size);

  printf("%s\t%s\t%s\t%s\n", "PID", "STATE", "CPU USAGE(%)", "COMMAND");

  iterate_processes(processes_id, processes_id_size, cpu_usage_list);

  free(processes_id);
  closedir(proc_dir);

  return EXIT_SUCCESS;
}
