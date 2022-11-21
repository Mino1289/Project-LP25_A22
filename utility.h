//
// Created by flassabe on 26/10/22.
//

#ifndef A2022_UTILITY_H
#define A2022_UTILITY_H

#include <stdbool.h>
#include <dirent.h>

char *concat_path(char *prefix, char *suffix, char *full_path);
bool directory_exists(char *path);
bool path_to_file_exists(char *path);
void sync_temporary_files(char *temp_dir);
struct dirent *next_dir(struct dirent *entry, DIR *dir);

#endif //A2022_UTILITY_H
