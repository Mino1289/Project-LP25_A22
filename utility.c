//
// Created by flassabe on 26/10/22.
//

#include "utility.h"

#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <libgen.h>
#include <unistd.h>
#include <fcntl.h>


#include <stdio.h>
//TODO: remove

#include "global_defs.h"

/*!
 * @brief cat_path concatenates two file system paths into a result. It adds the separation /  if required.
 * @param prefix first part of the complete path
 * @param suffix second part of the complete path
 * @param full_path resulting path
 * @return pointer to full_path if operation succeeded, NULL else
 */
char *concat_path(char *prefix, char *suffix, char *full_path) {
    if (prefix == NULL || suffix == NULL || full_path == NULL) {
        return NULL;
    }
    // Check if the total size + 2 (for the '\0' and maybe '/') is not too big
    if (strlen(prefix) + strlen(suffix) + 2 > STR_MAX_LEN) {
        return NULL;
    }
    strcpy(full_path, prefix);
    if (full_path[strlen(full_path) - 1] != '/') {
        strcat(full_path, "/");
    }
    strcat(full_path, suffix);
    return full_path;
}

/*!
 * @brief directory_exists tests if directory located at path exists
 * @param path the path whose existence to test
 * @return true if directory exists, false else
 */
bool directory_exists(char *path)
{
    if (!path) return false;
    DIR *dir = opendir(path);
    if (dir) {
        closedir(dir);
        return true;
    } else {
        closedir(dir);
        return false;
    }
    
}

/*!
 * @brief path_to_file_exists tests if a path leading to a file exists. It separates the path to the file from the
 * file name itself. For instance, path_to_file_exists("/var/log/Xorg.0.log") will test if /var/log exists and is a
 * directory.
 * @param path the path to the file
 * @return true if path to file exists, false else
 */
bool path_to_file_exists(char *path) {
    char* path_to_file = (char *) malloc(sizeof(char) * STR_MAX_LEN);
    path_to_file = realpath(path, path_to_file);
    bool exists = directory_exists(path_to_file);
    free(path_to_file);
    return exists;
}

/*!
 * @brief sync_temporary_files waits for filesystem syncing for a path
 * @param temp_dir the path to the directory to wait for
 * Use fsync and dirfd
 */
void sync_temporary_files(char *temp_dir) {
    int fd = open(temp_dir, O_RDONLY);
    fsync(fd);
    close(fd);
}

/*!
 * @brief next_dir returns the next directory entry that is not . or ..
 * @param entry a pointer to the current struct dirent in caller
 * @param dir a pointer to the already opened directory
 * @return a pointer to the next not . or .. directory, NULL if none remain
 */
struct dirent *next_dir(struct dirent *entry, DIR *dir) {
    do {
        entry = readdir(dir);
    } while (entry && (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")));
    return entry;
}
