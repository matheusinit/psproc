#ifndef FILE_H
#define FILE_H

#include <dirent.h>

char *get_file_content(char *filename, char *dir_path);

DIR *check_and_open_directory(char *path);

#endif
