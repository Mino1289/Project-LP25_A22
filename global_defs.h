//
// Created by flassabe on 13/10/22.
//

#ifndef A2022_GLOBAL_DEFS_H
#define A2022_GLOBAL_DEFS_H

#include <stdint.h>

#define STR_MAX_LEN 1024

#define ABS(a)      ((a) < 0 ? -(a) : (a))
#define MAX(a, b)   ((a + b + ABS(a-b)) / 2)

typedef struct _task {
    void (* task_callback)(struct _task *);
    char argument[2*STR_MAX_LEN]; // Can be extended based on actual task
} task_t;

#endif //A2022_GLOBAL_DEFS_H
