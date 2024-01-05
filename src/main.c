#include "file.h"
#include "process.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  int processes_id_size = 0;

  DIR *proc_dir = check_and_open_directory("/proc");

  char **processes_id = get_processes_ids(proc_dir, &processes_id_size);

  float *cpu_usage_list = calculate_cpu_usage(processes_id, processes_id_size);

  printf("%s\t%s\t%s\t%s\t%s\n", "PID", "STATE", "CPU USAGE(%)",
         "RESIDENT MEMORY", "COMMAND");

  struct process **processes = iterate_processes_and_get_process_info(
      processes_id, processes_id_size, cpu_usage_list);

  free(cpu_usage_list);
  free(processes_id);

  print_processes_info(processes, processes_id_size);

  for (int index = 0; index < processes_id_size; index++) {
    free(processes[index]->command);
    free(processes[index]);
  }
  free(processes);

  closedir(proc_dir);

  return EXIT_SUCCESS;
}
