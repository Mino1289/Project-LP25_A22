//
// Created by flassabe on 26/10/22.
//

#ifndef A2022_REDUCERS_H
#define A2022_REDUCERS_H

#include "global_defs.h"

typedef struct _recipient {
    char recipient_address[STR_MAX_LEN];
    uint32_t occurrences;
    struct _recipient *prev;
    struct _recipient *next;
} recipient_t;

typedef struct _sender {
    char sender_address[STR_MAX_LEN];
    recipient_t *head; // Head of recipient list
    recipient_t *tail; // Tail of recipient list
    struct _sender *prev;
    struct _sender *next;
} sender_t;

sender_t *add_source_to_list(sender_t *list, char *source_email);
void clear_sources_list(sender_t *list);
sender_t *find_source_in_list(sender_t *list, char *source_email);
void add_recipient_to_source(sender_t *source, char *recipient_email);

void files_list_reducer(char *data_source, char *temp_files, char *output_file);
void files_reducer(char *temp_file, char *output_file);

#endif //A2022_REDUCERS_H
