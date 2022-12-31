//
// Created by flassabe on 26/10/22.
//

#include "direct_fork.h"

#include <dirent.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

#include "analysis.h"
#include "utility.h"

/*!
 * @brief direct_fork_directories runs the directory analysis with direct calls to fork
 * @param data_source the data source directory with 150 directories to analyze (parallelize with fork)
 * @param temp_files the path to the temporary files directory
 * @param nb_proc the maximum number of simultaneous processes
 */
void direct_fork_directories(char *data_source, char *temp_files, uint16_t nb_proc) {
    // 1. Check parameters
    if (!directory_exists(data_source)) {
        printf("Error: data source directory does not exist.\n");
        return;
    }
    DIR *dir = opendir(data_source);
    // TODO: Memory leak
    if (!dir) {
        printf("Error: could not open data source directory.\n");
        closedir(dir);
        return;
    }
    uint16_t current_proc = 0;
    struct dirent *entry = readdir(dir);
    // 2. Iterate over directories (ignore . and ..)
    while (entry != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            char entry_path[STR_MAX_LEN];
            concat_path(data_source, entry->d_name, entry_path);
            if (directory_exists(entry_path)) {
                // 3 bis: if max processes count already run, wait for one to end before starting a task.
                if (current_proc >= nb_proc) {
                    wait(NULL);
                    --current_proc;
                }
                // 3. fork and start a task on current directory.
                pid_t pid = fork();
                if (pid == 0) {
                    // child process
                    char output_file[STR_MAX_LEN]; 
                    concat_path(temp_files, entry->d_name, output_file);

                    directory_task_t *t = (directory_task_t *) malloc(sizeof(task_t));

                    t->task_callback = process_directory;
                    strncpy(t->object_directory, entry_path, STR_MAX_LEN);
                    strncpy(t->temporary_directory, output_file, STR_MAX_LEN);
                    t->task_callback((task_t*) t);

                    free(t);
                    exit(EXIT_SUCCESS);
                } else if (pid > 0) {
                    // parent process
                    ++current_proc;
                } else {
                    // error
                    printf("Error: could not fork.\n");
                    closedir(dir);
                    exit(EXIT_FAILURE);
                }
            }
        }
        entry = next_dir(entry, dir);
    }
    for (int i = 0; i < current_proc; ++i) {
        wait(NULL);
    }
    // 4. Cleanup
    closedir(dir);
    //TODO: Check for memory leaks
}

/*!
 * @brief direct_fork_files runs the files analysis with direct calls to fork
 * @param data_source the data source containing the files (step1_output)
 * @param temp_files the temporary files to write the output (step2_output)
 * @param nb_proc the maximum number of simultaneous processes
 */
void direct_fork_files(char *data_source, char *temp_file, uint16_t nb_proc) {
    // 1. Check parameters
    if (!path_to_file_exists(data_source)) {
        printf("Error: %s does not exist.\n", data_source);
        return;
    }
    if (temp_file == NULL) {
        printf("Error: %s does not exist.\n", temp_file);
        return;
    }
    uint16_t current_proc = 0;
    // 2. Iterate over files in files list (step1_output)
    FILE* files_list = fopen(data_source, "r");
    if (files_list == NULL) {
        fclose(files_list);
        printf("Error: could not open %s.\n", data_source);
        return;
    }
    char file_path[STR_MAX_LEN];
    while (fgets(file_path, STR_MAX_LEN, files_list) != NULL) {
        file_path[strlen(file_path) - 1] = '\0';
        if (path_to_file_exists(file_path)) {
            if (current_proc > nb_proc) {
                // 3 bis: if max processes count already run, wait for one to end before starting a task.
                wait(NULL);
                --current_proc;
            }
            // 3. fork and start a task on current file.
            pid_t pid = fork();
            if (pid == 0) {
                // child process
                file_task_t *t = (file_task_t *) malloc(sizeof(task_t));

                t->task_callback = process_file;
                
                strncpy(t->object_file, file_path, STR_MAX_LEN);
                strncpy(t->temporary_directory, temp_file, STR_MAX_LEN);
                t->task_callback((task_t*) t);

                free(t);
                fclose(files_list);
                exit(EXIT_SUCCESS);
            } else if (pid > 0) {
                // parent process
                ++current_proc;
            } else {
                // error
                printf("Error: could not fork.\n");
                fclose(files_list);
                exit(EXIT_FAILURE);
            }
        }
    }

    // 4. Cleanup
    
    for (int i = 0; i < current_proc; ++i) {
        wait(NULL);
    }
    fclose(files_list);
    //TODO: Check for memory leaks
}
