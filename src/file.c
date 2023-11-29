#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file.h"

char *get_file_content(char *filename, char *dir_path) {
  int file_content_buffer_capacity = 16;
  char *file_content = calloc(file_content_buffer_capacity + 1, sizeof(char));
  file_content[file_content_buffer_capacity] = '\0';

  int file_content_buffer_size = 0;

  size_t stat_path_length = strlen(dir_path) + strlen(filename) + 1;

  char *pid_stat_path = calloc(stat_path_length + 1, sizeof(char));
  pid_stat_path[stat_path_length] = '\0';

  strcpy(pid_stat_path, dir_path);
  strcat(pid_stat_path, "/");
  strcat(pid_stat_path, filename);

  FILE *exe_file = fopen(pid_stat_path, "r");

  size_t bytes_read;
  int buffer_capacity = sizeof(char) * 16;
  char *buffer = calloc(buffer_capacity + 1, sizeof(char));

  if (exe_file == NULL) {
    printf("Error opening file %s\n", filename);

    exit(EXIT_FAILURE);
  }

  while ((bytes_read = fread(buffer, 1, buffer_capacity, exe_file)) > 0) {
    if (buffer_capacity >= 0 && bytes_read < (size_t)buffer_capacity) {
      buffer[bytes_read] = '\0';
    }

    strcat(file_content, buffer);
    file_content_buffer_size += bytes_read;

    if (file_content_buffer_size >= file_content_buffer_capacity) {
      file_content_buffer_capacity *= 2;
      file_content = realloc(file_content, file_content_buffer_capacity + 1);
    }
  }

  free(buffer);

  fclose(exe_file);

  return file_content;
}

DIR *check_and_open_directory(char *path) {
  DIR *directory = opendir(path);

  if (directory == NULL) {
    printf("Error opening directory\n");
    exit(EXIT_FAILURE);
  };

  return directory;
}
