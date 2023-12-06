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

  printf("%s\t%s\t%s\n", "PID", "STATE", "COMMAND");

  for (int index = 0; index < 1; index++) {
    char *selected_pid = processes_id[index];

    int cpu_usage = calculate_cpu_usage(selected_pid);

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
            file_content, &file_content_array_size, " ", 52);

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

    // printf("%s\t%s\t%s\n", pid, state, command);
  }

  free(processes_id);
  closedir(proc_dir);

  return EXIT_SUCCESS;
}
