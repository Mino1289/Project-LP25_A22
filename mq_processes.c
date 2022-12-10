//
// Created by flassabe on 10/11/22.
//

#include "mq_processes.h"

#include <sys/msg.h>

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
// #include <bits/types/sig_atomic_t.h>
#include <stdio.h>

#include "utility.h"
#include "analysis.h"

/*!
 * @brief make_message_queue creates the message queue used for communications between parent and worker processes
 * @return the file descriptor of the message queue
 */

int make_message_queue()
{
    int mq = msgget(IPC_PRIVATE, 0600);
    if (mq == -1)
    {
        perror("msgget");
        exit(EXIT_FAILURE);
    }
    return mq;
}

/*!
 * @brief close_message_queue closes a message queue
 * @param mq the descriptor of the MQ to close
 */
void close_message_queue(int mq)
{
    if (msgctl(mq, IPC_RMID, NULL) == -1)
    {
        perror("msgctl");
        exit(EXIT_FAILURE);
    }
}

/*!
 * @brief child_process is the function handling code for a child
 * @param mq message queue descriptor used to communicate with the parent
 */
void child_process(int mq)
{
    // 1. Endless loop (interrupted by a task whose callback is NULL)
    // 2. Upon reception of a task: check is not NULL
    // 2 bis. If not NULL -> execute it and notify parent
    // 2 ter. If NULL -> leave loop
    // 3. Cleanup
}

/*!
 * @brief mq_make_processes makes a processes pool used for tasks execution
 * @param config a pointer to the program configuration (with all parameters, inc. processes count)
 * @param mq the identifier of the message queue used to communicate between parent and children (workers)
 * @return a malloc'ed array with all children PIDs
 */
pid_t *mq_make_processes(configuration_t *config, int mq) {
    return NULL;
}

/*!
 * @brief close_processes commands all workers to terminate
 * @param config a pointer to the configuration
 * @param mq the message queue to communicate with the workers
 * @param children the array of children's PIDs
 */
void close_processes(configuration_t *config, int mq, pid_t children[]) {
}

/*!
 * @brief send_task_to_mq sends a directory task to a worker through the message queue. Directory task's object is
 * data_source/target_dir, temp output file is temp_files/target_dir. Task is sent through MQ with topic equal to
 * the worker's PID
 * @param data_source the data source directory
 * @param temp_files the temporary files directory
 * @param target_dir the name of the target directory
 * @param mq the MQ descriptor
 * @param worker_pid the worker PID
 */
void send_task_to_mq(char data_source[], char temp_files[], char target_dir[], int mq, pid_t worker_pid) {
}

/*!
 * @brief send_file_task_to_mq sends a file task to a worker. It operates similarly to @see send_task_to_mq
 * @param data_source the data source directory
 * @param temp_files the temporary files directory
 * @param target_file the target filename
 * @param mq the MQ descriptor
 * @param worker_pid the worker's PID
 */
void send_file_task_to_mq(char data_source[], char temp_files[], char target_file[], int mq, pid_t worker_pid) {
}

/*!
 * @brief mq_process_directory root function for parallelizing directory analysis over workers. Must keep track of the
 * tasks count to ensure every worker handles one and only one task. Relies on two steps: one to fill all workers with
 * a task each, then, waiting for a child to finish its task before sending a new one.
 * @param config a pointer to the configuration with all relevant path and values
 * @param mq the MQ descriptor
 * @param children the children's PIDs used as MQ topics number
 */
void mq_process_directory(configuration_t *config, int mq, pid_t children[]) {
    // 1. Check parameters
    // 2. Iterate over children and provide one directory to each
    // 3. Loop while there are directories to process, and while all workers are processing
    // 3 bis. For each worker finishing its task: send a new one if any task is left, keep track of running workers else
    // 4. Cleanup
}

/*!
 * @brief mq_process_files root function for parallelizing files analysis over workers. Operates as
 * @see mq_process_directory to limit tasks to one on each worker.
 * @param config a pointer to the configuration with all relevant path and values
 * @param mq the MQ descriptor
 * @param children the children's PIDs used as MQ topics number
 */
void mq_process_files(configuration_t *config, int mq, pid_t children[]) {
    // 1. Check parameters
    // 2. Iterate over children and provide one file to each
    // 3. Loop while there are files to process, and while all workers are processing
    // 3 bis. For each worker finishing its task: send a new one if any task is left, keep track of running workers else
    // 4. Cleanup
}
