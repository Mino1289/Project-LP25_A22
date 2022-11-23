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
        {.name="cpu-multiplier",.has_arg=1,.flag=0,.val='c'},
        {.name=0,.has_arg=0,.flag=0,.val=0},
    };
    int opt;

    configuration_t *configuration = base_configuration;

    while ((opt = getopt_long(argc, argv, "vd:t:o:c:", my_opts, NULL)) != EOF) {
        switch (opt) {
            case 'v':
                configuration->is_verbose = true;
                break;
            
            case 'd':
                strncpy(configuration->data_path, optarg, STR_MAX_LEN);
                break;

            case 't':
                strncpy(configuration->temporary_directory, optarg, STR_MAX_LEN);
                break;
            
            case 'o':
                strncpy(configuration->output_file, optarg, STR_MAX_LEN);
                break;

            case 'c':
                configuration->cpu_core_multiplier = atoi(optarg);
                break;
                
            default:
                break;
        }
    }
    if (is_configuration_valid(configuration)) {
        return configuration;
    } else {
        return base_configuration;
    }
}

/*!
 * @brief skip_spaces advances a string pointer to the first non-space character
 * @param str the pointer to advance in a string
 * @return a pointer to the first non-space character in str
 */
char *skip_spaces(char *str) {
    return str;
}

/*!
 * @brief check_equal looks for an optional sequence of spaces, an equal '=' sign, then a second optional sequence
 * of spaces
 * @param str the string to analyze
 * @return a pointer to the first non-space character after the =, NULL if no equal was found
 */
char *check_equal(char *str) {
    return str;
}

/*!
 * @brief get_word extracts a word (a sequence of non-space characters) from the source
 * @param source the source string, where to find the word
 * @param target the target string, where to copy the word
 * @return a pointer to the character after the end of the extracted word
 */
char *get_word(char *source, char *target) {
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
bool is_configuration_valid(configuration_t *configuration) {
    if (directory_exists(configuration->data_path) && 
        directory_exists(configuration->temporary_directory) && 
        path_to_file_exists(configuration->output_file) && 
        ((configuration->cpu_core_multiplier <= 10) &&
         (configuration->cpu_core_multiplier >= 1))) {

        return true;
    }
    return false;
}