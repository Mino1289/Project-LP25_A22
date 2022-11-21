//
// Created by flassabe on 27/10/22.
//

#ifndef A2022_FIFO_PROCESSES_H
#define A2022_FIFO_PROCESSES_H

#include "global_defs.h"
#include <unistd.h>
#include <stdio.h>

void make_fifos(uint16_t processes_count, char *file_format);
void erase_fifos(uint16_t processes_count, char *file_format);
pid_t *make_processes(uint16_t processes_count);
int *open_fifos(uint16_t processes_count, char *file_format, int flags);
void close_fifos(uint16_t processes_count, int*files);
void shutdown_processes(uint16_t processes_count, int *fifos);

void fifo_process_directory(char *data_source, char *temp_files, int *notify_fifos, int *command_fifos, uint16_t nb_proc);
void fifo_process_files(char *data_source, char *temp_files, int *notify_fifos, int *command_fifos, uint16_t nb_proc);

#endif //A2022_FIFO_PROCESSES_H
