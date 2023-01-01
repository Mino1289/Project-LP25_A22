//
// Created by flassabe on 10/11/22.
//

#ifndef A2022_MQ_PROCESSES_H
#define A2022_MQ_PROCESSES_H

#include <stdint.h>
#include <sys/types.h>

#include "configuration.h"

typedef struct {
    long mtype;
    char mtext[sizeof(task_t)];
} mq_message_t;

int make_message_queue();
void close_message_queue(int mq);

void child_process(int mq);
pid_t *mq_make_processes(configuration_t *config, int mq);
void close_processes(configuration_t *config, int mq, pid_t children[]);
void mq_process_directory(configuration_t *config, int mq, pid_t children[]);
void mq_process_files(configuration_t *config, int mq, pid_t children[]);

#endif //A2022_MQ_PROCESSES_H
