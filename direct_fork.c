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
    // 2. Iterate over directories (ignore . and ..)
    // 3. fork and start a task on current directory.
    // 3 bis: if max processes count already run, wait for one to end before starting a task.
    // 4. Cleanup
}

/*!
 * @brief direct_fork_files runs the files analysis with direct calls to fork
 * @param data_source the data source containing the files
 * @param temp_files the temporary files to write the output (step2_output)
 * @param nb_proc the maximum number of simultaneous processes
 */
void direct_fork_files(char *data_source, char *temp_files, uint16_t nb_proc) {
    // 1. Check parameters
    // 2. Iterate over files in files list (step1_output)
    // 3. fork and start a task on current file.
    // 3 bis: if max processes count already run, wait for one to end before starting a task.
    // 4. Cleanup
}