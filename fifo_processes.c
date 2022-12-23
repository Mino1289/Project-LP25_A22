//
// Created by flassabe on 27/10/22.
//

#include "fifo_processes.h"

#include "global_defs.h"
// #include <malloc.h>
#include <sys/stat.h>
#include <signal.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#include "analysis.h"
#include "utility.h"

/*!
 * @brief make_fifos creates FIFOs for processes to communicate with their parent
 * @param processes_count the number of FIFOs to create
 * @param file_format the filename format, e.g. fifo-out-%d, used to name the FIFOs
 */
void make_fifos(uint16_t processes_count, char *file_format) {
    
    char buffer[STR_MAX_LEN];
    int i = 0; 

    while(i<processes_count){
        sprintf(buffer,"%s%d",file_format,i);
        if(mkfifo(buffer,0666) == -1){
            if(errno != EEXIST){
                printf("Could not create a fifo file\n");
                exit(EXIT_FAILURE);
            }
        };
        i++;
    }
}

/*!
 * @brief erase_fifos erases FIFOs used for processes communications with the parent
 * @param processes_count the number of FIFOs to destroy
 * @param file_format the filename format, e.g. fifo-out-%d, used to name the FIFOs
 */
void erase_fifos(uint16_t processes_count, char *file_format) {
    
    char buffer[STR_MAX_LEN];
    int i = 0;

     while(i<processes_count){
        sprintf(buffer,"%s%d",file_format,i);
        if(remove(buffer) == -1){
            fprintf(stderr,"Could not delete the fifo");
        };
        i++;
    }
}


/*!
 * @brief make_processes creates processes and starts their code (waiting for commands)
 * @param processes_count the number of processes to create
 * @return a malloc'ed array with the PIDs of the created processes
 */
pid_t *make_processes(uint16_t processes_count) {
    // 1. Create PIDs array
    // 2. Loop over processes_count to fork
    // 2 bis. in fork child part, open reading and writing FIFOs, and start listening on reading FIFO
    // 3. Upon reception, apply task
    // 3 bis. If task has a NULL callback, terminate process (don't forget cleanup).
    return NULL;
}

/*!
 * @brief open_fifos opens FIFO from the parent's side
 * @param processes_count the number of FIFOs to open (must be created before)
 * @param file_format the name pattern of the FIFOs
 * @param flags the opening mode for the FIFOs
 * @return a malloc'ed array of opened FIFOs file descriptors
 */
int *open_fifos(uint16_t processes_count, char *file_format, int flags) {
    
    int* file_descriptor = (int*)malloc(sizeof(int)*processes_count);
    char buffer[STR_MAX_LEN];

    for(int i = 0 ; i<processes_count; i++ ){
        sprintf(buffer,"%s%d",file_format,i);
        file_descriptor[i]= open(buffer,flags); 
    }

    return file_descriptor;
}

/*!
 * @brief close_fifos closes FIFOs opened by the parent
 * @param processes_count the number of FIFOs to close
 * @param files the array of opened FIFOs as file descriptors
 */
void close_fifos(uint16_t processes_count, int *files) {
     
  for (int i = 0; i < processes_count; i++) {
    if (files[i] >= 0) {
        if (close(files[i]) != 0) {
        fprintf(stderr, "Error, cannot close fifo %d\n", i);
      }
    }
  }

  free(files);
}

/*!
 * @brief shutdown_processes terminates all worker processes by sending a task with a NULL callback
 * @param processes_count the number of processes to terminate
 * @param fifos the array to the output FIFOs (used to command the processes) file descriptors
 */
void shutdown_processes(uint16_t processes_count, int *fifos) {
    // 1. Loop over processes_count
    // 2. Create an empty task (with a NULL callback)
    // 3. Send task to current process
}

/*!
 * @brief prepare_select prepares fd_set for select with all file descriptors to look at
 * @param fds the fd_set to initialize
 * @param filesdes the array of file descriptors
 * @param nb_proc the number of processes (elements in the array)
 * @return the maximum file descriptor value (as used in select)
 */
int prepare_select(fd_set *fds, const int *filesdes, uint16_t nb_proc) {
    return 1;
}

/*!
 * @brief send_task sends a directory task to a child process. Must send a directory command on object directory
 * data_source/dir_name, to write the result in temp_files/dir_name. Sends on FIFO with FD command_fd
 * @param data_source the data source with directories to analyze
 * @param temp_files the temporary output files directory
 * @param dir_name the current dir name to analyze
 * @param command_fd the child process command FIFO file descriptor
 */
void send_task(char *data_source, char *temp_files, char *dir_name, int command_fd) {
}

/*!
 * @brief fifo_process_directory is the main function to distribute directory analysis to worker processes.
 * @param data_source the data source with the directories to analyze
 * @param temp_files the temporary files directory
 * @param notify_fifos the FIFOs on which to read for workers to notify end of tasks
 * @param command_fifos the FIFOs on which to send tasks to workers
 * @param nb_proc the maximum number of simultaneous tasks, = to number of workers
 * Uses @see send_task
 */
void fifo_process_directory(char *data_source, char *temp_files, int *notify_fifos, int *command_fifos, uint16_t nb_proc) {
    // 1. Check parameters
    // 2. Iterate over directories (ignore . and ..)
    // 3. Send a directory task to each running worker process
    // 4. Iterate over remaining directories by waiting for a process to finish its task before sending a new one.
    // 5. Cleanup
}

/*!
 * @brief fifo_process_files is the main function to distribute files analysis to worker processes.
 * @param data_source the data source with the files to analyze
 * @param temp_files the temporary files directory (step1_output is here)
 * @param notify_fifos the FIFOs on which to read for workers to notify end of tasks
 * @param command_fifos the FIFOs on which to send tasks to workers
 * @param nb_proc  the maximum number of simultaneous tasks, = to number of workers
 */
void fifo_process_files(char *data_source, char *temp_files, int *notify_fifos, int *command_fifos, uint16_t nb_proc) {
    // 1. Check parameters
    // 2. Iterate over files in step1_output
    // 3. Send a file task to each running worker process (you may create a utility function for this)
    // 4. Iterate over remaining files by waiting for a process to finish its task before sending a new one.
    // 5. Cleanup
}
