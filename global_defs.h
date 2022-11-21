//
// Created by flassabe on 13/10/22.
//

#ifndef A2022_GLOBAL_DEFS_H
#define A2022_GLOBAL_DEFS_H

#include <stdint.h>

#define STR_MAX_LEN 1024

typedef struct _task {
    void (* task_callback)(struct _task *);
    char argument[2*STR_MAX_LEN]; // Can be extended based on actual task
} task_t;

#endif //A2022_GLOBAL_DEFS_H
