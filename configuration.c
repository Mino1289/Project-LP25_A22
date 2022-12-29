//
// Created by flassabe on 14/10/22.
//

#include "configuration.h"

#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "utility.h"

/*!
 * @brief make_configuration makes the configuration from the program parameters. CLI parameters are applied after
 * file parameters. You shall keep two configuration sets: one with the default values updated by file reading (if
 * configuration file is used), a second one with CLI parameters, to overwrite the first one with its values if any.
 * @param base_configuration a pointer to the base configuration to be updated
 * @param argv the main argv
 * @param argc the main argc
 * @return the pointer to the updated base configuration
 */
configuration_t *make_configuration(configuration_t *base_configuration, char *argv[], int argc) {
    struct option my_opts[] = {
        {.name="verbose",.has_arg=0,.flag=0,.val='v'},
        {.name="data-source",.has_arg=1,.flag=0,.val='d'},
        {.name="temporary-directory",.has_arg=1,.flag=0,.val='t'},
        {.name="output-file",.has_arg=1,.flag=0,.val='o'},
        {.name="cpu-multiplier",.has_arg=1,.flag=0,.val='n'},
        {.name="config-file",.has_arg=1,.flag=0,.val='f'},
        {.name=0,.has_arg=0,.flag=0,.val=0},
    };
    int opt;

    while ((opt = getopt_long(argc, argv, "vd:t:o:n:f:", my_opts, NULL)) != EOF) {
        switch (opt) {
            case 'v':
                base_configuration->is_verbose = true;
                break;
            
            case 'd':
                strncpy(base_configuration->data_path, optarg, STR_MAX_LEN);
                break;

            case 't':
                strncpy(base_configuration->temporary_directory, optarg, STR_MAX_LEN);
                break;
            
            case 'o':
                strncpy(base_configuration->output_file, optarg, STR_MAX_LEN);
                break;

            case 'c':
                base_configuration->cpu_core_multiplier = atoi(optarg);
                break;
            case 'f':
                base_configuration = read_cfg_file(base_configuration, optarg);
                return base_configuration;
                break;
            default:
                break;
        }
    }
    return base_configuration;
}

/*!
 * @brief skip_spaces advances a string pointer to the first non-space character
 * @param str the pointer to advance in a string
 * @return a pointer to the first non-space character in str
 */
char *skip_spaces(char *str) {
    while (isspace(*str)) {
        str++;
    }
    return str;
}

/*!
 * @brief check_equal looks for an optional sequence of spaces, an equal '=' sign, then a second optional sequence
 * of spaces
 * @param str the string to analyze
 * @return a pointer to the first non-space character after the =, NULL if no equal was found
 */
char *check_equal(char *str) {
    str = skip_spaces(str);
    if (*str == '=') {
        ++str;
        str = skip_spaces(str);
        return str;
    } else {
        return NULL;
    }
}

/*!
 * @brief get_word extracts a word (a sequence of non-space characters) from the source
 * @param source the source string, where to find the word
 * @param target the target string, where to copy the word
 * @return a pointer to the character after the end of the extracted word
 */
char *get_word(char *source, char *target) {
    while (*source != '\0' && !isspace(*source)) {
        *target = *source;
        ++target;
        ++source;
    }
    *target = '\0';
    return source;
}

/*!
 * @brief read_cfg_file reads a configuration file (with key = value lines) and extracts all key/values for
 * configuring the program (data_path, output_file, temporary_directory, is_verbose, cpu_core_multiplier)
 * @param base_configuration a pointer to the configuration to update and return
 * @param path_to_cfg_file the path to the configuration file
 * @return a pointer to the base configuration after update, NULL is reading failed.
 */
configuration_t *read_cfg_file(configuration_t *base_configuration, char *path_to_cfg_file) {
    if (base_configuration == NULL || path_to_cfg_file == NULL) {
        return NULL;
    }
    FILE *cfg_file = fopen(path_to_cfg_file, "r");
    if (cfg_file == NULL) {
        return base_configuration;
    }
    
    while (!feof(cfg_file)) {
        char line[STR_MAX_LEN], key[STR_MAX_LEN], value[STR_MAX_LEN];
        fgets(line, STR_MAX_LEN, cfg_file);
        strcpy(line, skip_spaces(line));
        strcpy(line, get_word(line, key));
        strcpy(line, check_equal(line));
        strcpy(line, skip_spaces(line));
        strcpy(line, get_word(line, value));

        if (strcmp(key, "data_path") == 0) {
            strncpy(base_configuration->data_path, value, STR_MAX_LEN);
        } else if (strcmp(key, "temporary_directory") == 0) {
            strncpy(base_configuration->temporary_directory, value, STR_MAX_LEN);
        } else if (strcmp(key, "output_file") == 0) {
            strncpy(base_configuration->output_file, value, STR_MAX_LEN);
        } else if (strcmp(key, "cpu_core_multiplier") == 0) {
            base_configuration->cpu_core_multiplier = atoi(value);
        } else if (strcmp(key, "is_verbose") == 0) {
            if (strcmp(value, "true") == 0 || strcmp(value, "1") == 0 || strcmp(value, "yes") == 0) {
                base_configuration->is_verbose = true;
            } else {
                base_configuration->is_verbose = false;
            }
        } else {
            printf("Unknown key: %s\n", key);
        }
    }

    return base_configuration;
}

/*!
 * @brief display_configuration displays the content of a configuration
 * @param configuration a pointer to the configuration to print
 */
void display_configuration(configuration_t *configuration) {
    printf("Current configuration:\n");
    printf("\tData source: %s\n", configuration->data_path);
    printf("\tTemporary directory: %s\n", configuration->temporary_directory);
    printf("\tOutput file: %s\n", configuration->output_file);
    printf("\tVerbose mode is %s\n", configuration->is_verbose ? "on" : "off");
    printf("\tCPU multiplier is %d\n", configuration->cpu_core_multiplier);
    printf("\tProcess count is %d\n", configuration->process_count);
    printf("End configuration\n");
}

/*!
 * @brief is_configuration_valid tests a configuration to check if it is executable (i.e. data directory and temporary
 * directory both exist, and path to output file exists @see directory_exists and path_to_file_exists in utility.c)
 * @param configuration the configuration to be tested
 * @return true if configuration is valid, false else
 */
bool is_configuration_valid(configuration_t *configuration)
{    
    if (directory_exists(configuration->data_path) && 
        directory_exists(configuration->temporary_directory) && 
        path_to_file_exists(configuration->output_file) && 
        ((configuration->cpu_core_multiplier <= 10) &&
         (configuration->cpu_core_multiplier >= 1))) {

        return true;
    }
    return false;
}
