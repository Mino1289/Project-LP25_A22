//
// Created by flassabe on 26/10/22.
//

#ifndef A2022_ANALYSIS_H
#define A2022_ANALYSIS_H

#include "global_defs.h"
#include <stdio.h>

typedef struct _simple_recipient {
    char email[STR_MAX_LEN];
    struct _simple_recipient *next;
} simple_recipient_t;

typedef struct {
    void (* task_callback)(task_t *);
    char object_directory[STR_MAX_LEN];
    char temporary_directory[STR_MAX_LEN];
} directory_task_t;

typedef struct {
    void (* task_callback)(task_t *);
    char object_file[STR_MAX_LEN];
    char temporary_directory[STR_MAX_LEN];
} file_task_t;

void parse_dir(char *path, FILE *output_file);
void parse_file(char *filepath, char *output);

void process_directory(task_t *task);
void process_file(task_t *task);

#endif //A2022_ANALYSIS_H
