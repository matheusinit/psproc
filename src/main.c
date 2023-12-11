#include "file.h"
#include "process.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  struct dirent *files;
  int array_capacity = 1000;
  int processes_id_size = 0;
  char **processes_id = malloc(sizeof(char *) * array_capacity);

  DIR *proc_dir = check_and_open_directory("/proc");

  processes_id = get_processes_ids(proc_dir, &processes_id_size);

  float *cpu_usage_list = calculate_cpu_usage(processes_id, processes_id_size);

  printf("%s\t%s\t%s\t%s\n", "PID", "STATE", "CPU USAGE(%)", "COMMAND");

  for (int index = 0; index < processes_id_size; index++) {
    struct process current_process;

    current_process.cpu_usage = cpu_usage_list[index];

    char *selected_pid = processes_id[index];

    current_process.pid_path = get_path_for_process(selected_pid);

    DIR *pid_dir = check_and_open_directory(current_process.pid_path);

    while ((files = readdir(pid_dir))) {

      char **file_content_array;

      if (strcmp(files->d_name, ".") == 0) {
        continue;
      }

      if (strcmp(files->d_name, "..") == 0) {
        continue;
      }

      if (strcmp(files->d_name, "stat") == 0) {

        char *file_content =
            get_file_content(files->d_name, current_process.pid_path);

        int file_content_array_size = 0;
        file_content_array = split_string_by_delimiter(
            file_content, &file_content_array_size, " ", 52);

        current_process.pid = file_content_array[0];
        current_process.state = file_content_array[2];
      }

      if (strcmp(files->d_name, "cmdline") == 0) {
        char *file_content =
            get_file_content(files->d_name, current_process.pid_path);

        current_process.command = file_content;

        if (strcmp(current_process.command, "") == 0) {
          current_process.command = file_content_array[1];
        }
      }
    }

    free(current_process.pid_path);
    closedir(pid_dir);

    printf("%s\t%s\t%.2f\t%s\n", current_process.pid, current_process.state,
           current_process.cpu_usage, current_process.command);
  }

  free(processes_id);
  closedir(proc_dir);

  return EXIT_SUCCESS;
}
