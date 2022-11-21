//
// Created by flassabe on 26/10/22.
//

#ifndef A2022_DIRECT_FORK_H
#define A2022_DIRECT_FORK_H

#include "global_defs.h"

void direct_fork_directories(char *data_source, char *temp_files, uint16_t nb_proc);
void direct_fork_files(char *data_source, char *temp_files, uint16_t nb_proc);

#endif //A2022_DIRECT_FORK_H
