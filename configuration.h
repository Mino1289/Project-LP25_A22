//
// Created by flassabe on 14/10/22.
//

#ifndef A2022_CONFIGURATION_H
#define A2022_CONFIGURATION_H

#include <stdbool.h>
#include <stdint.h>

#include "global_defs.h"

typedef struct {
    char data_path[STR_MAX_LEN];
    char temporary_directory[STR_MAX_LEN];
    char output_file[STR_MAX_LEN];
    bool is_verbose;
    uint8_t cpu_core_multiplier;
    uint16_t process_count;
} configuration_t;

configuration_t *make_configuration(configuration_t *base_configuration, char *argv[], int argc);
configuration_t *read_cfg_file(configuration_t *base_configuration, char *path_to_cfg_file);
void display_configuration(configuration_t *configuration);
bool is_configuration_valid(configuration_t *configuration);

#endif //A2022_CONFIGURATION_H
