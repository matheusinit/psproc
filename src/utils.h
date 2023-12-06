#ifndef UTILS_H
#define UTILS_H

int has_letters_in_string(char *string);

char **split_string_by_delimiter(char *string, int *size, char *delimiter,
                                 int capacity);

void print_directory_children_from_array(char **array, int array_size);

#endif
