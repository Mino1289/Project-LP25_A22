//
// Created by flassabe on 26/10/22.
//

#include "reducers.h"

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "global_defs.h"
#include "utility.h"

/*!
 * @brief add_source_to_list adds an e-mail to the sources list. If the e-mail already exists, do not add it.
 * @param list the list to update
 * @param source_email the e-mail to add as a string
 * @return a pointer to the updated beginning of the list
 */
sender_t* add_source_to_list(sender_t* list, char* source_email) {
    sender_t* temp_sender = find_source_in_list(list, source_email);

    if (temp_sender == NULL) {
        sender_t* temp_sender = (sender_t*)malloc(sizeof(sender_t));
        strncpy(temp_sender->sender_address, source_email, STR_MAX_LEN);

        if (list != NULL) {
            temp_sender->next = list;
            list->prev = temp_sender;
        } else {
            temp_sender->next = NULL;
            temp_sender->prev = NULL;
        }

        temp_sender->head = NULL;
        temp_sender->tail = NULL;

        return temp_sender;
    } 
    return list;
}

/*!
 * @brief clear_sources_list clears the list of e-mail sources (therefore clearing the recipients of each source)
 * @param list a pointer to the list to clear
 */
void clear_sources_list(sender_t* list){
    sender_t* temp = list;
    while (temp != NULL) {
        list = list->next;
        if (temp->head != NULL) {
            while (temp->head->next != NULL) {
                temp->head = temp->head->next;
                free(temp->head->prev);
            }
            free(temp->head);
        }
        free(temp);
        temp = list;
    }
}
/*!
 * @brief find_source_in_list looks for an e-mail address in the sources list and returns a pointer to it.
 * @param list the list to look into for the e-mail
 * @param source_email the e-mail as a string to look for
 * @return a pointer to the matching source, NULL if none exists
 */
sender_t* find_source_in_list(sender_t* list, char* source_email){
    sender_t* temp = list;
    while (temp != NULL && strcmp(temp->sender_address, source_email) != 0) {
        temp = temp->next;
    }
    return temp;
}

/*!
 * @brief add_recipient_to_source adds or updates a recipient in the recipients list of a source. It looks for
 * the e-mail in the recipients list: if it is found, its occurrences is incremented, else a new recipient is created
 * with its occurrences = to 1.
 * @param source a pointer to the source to add/update the recipient to
 * @param recipient_email the recipient e-mail to add/update as a string
 */
void add_recipient_to_source(sender_t* source, char* recipient_email) {
    if (!source) return;

    if (!source->head) {
        recipient_t* new_recipient = (recipient_t*)malloc(sizeof(recipient_t));
        strncpy(new_recipient->recipient_address, recipient_email, STR_MAX_LEN);
        new_recipient->occurrences = 1;
        source->head = new_recipient;
            source->tail = new_recipient;
            new_recipient->next = NULL;
            new_recipient->prev = NULL;
    }

    recipient_t* temp = source->head;

    while (temp != NULL && strcmp(temp->recipient_address, recipient_email) != 0) {
        temp = temp->next;
    }
    if (temp != NULL) {
        ++(temp->occurrences);
    } else {
        recipient_t* new_recipient = (recipient_t*)malloc(sizeof(recipient_t));
        strncpy(new_recipient->recipient_address, recipient_email, STR_MAX_LEN);
        new_recipient->occurrences = 1;
        
        new_recipient->next = source->head;
        source->head->prev = new_recipient;
        new_recipient->prev = NULL;
        source->head = new_recipient;
    }
}

/*!
 * @brief files_list_reducer is the first reducer. It uses concatenates all temporary files from the first step into
 * a single file. Don't forget to sync filesystem before leaving the function.
 * @param data_source the data source directory (its directories have the same names as the temp files to concatenate)
 * @param temp_files the temporary files directory, where to read files to be concatenated
 * @param output_file path to the output file (default name is step1_output, but we'll keep it as a parameter).
 */
void files_list_reducer(char* data_source, char* temp_files, char* output_file)
{
    // Open the output file for writing
    FILE* output = fopen(output_file, "w");
    if (!output) {
        perror("Cannot open output_file");
        exit(EXIT_FAILURE);
    }

    // Open the temporary files directory
    DIR* temp_dir = opendir(temp_files);
    if (!temp_dir) {
        perror("Cannot open directory");
        exit(EXIT_FAILURE);
    }

    // Read the entries in the directory
    char buffer[STR_MAX_LEN];
    struct dirent* entry;
    while ((entry = readdir(temp_dir)) != NULL) {
        
        // Skip the ".", ".." and the output entries
        if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
            

            // Construct the full path to the temporary file
            char temp_file_path[STR_MAX_LEN];
            concat_path(temp_files, entry->d_name, temp_file_path);
            if (strncmp(temp_file_path, output_file, STR_MAX_LEN)) {
                // Open the temporary file for reading
                FILE* temp_file = fopen(temp_file_path, "r");
                if (!temp_file) {
                    perror("Cannot open file");
                    exit(EXIT_FAILURE);
                }

                while(fgets(buffer, sizeof(buffer), temp_file) != NULL){
                    // buffer[strlen(buffer)-1] = '\0';
                    fputs(buffer, output);
                }
                // Close the temporary file
                fclose(temp_file);
            }
        }
    }

    // Close the output file and temporary files directory
    fclose(output);
    closedir(temp_dir);

    //sync the system
    sync();
}

/*!
 * @brief files_reducer opens the second temporary output file (default step2_output) and collates all sender/recipient
 * information as defined in the project instructions. Stores data in a double level linked list (list of source e-mails
 * containing each a list of recipients with their occurrences).
 * @param temp_file path to temp output file
 * @param output_file final output file to be written by your function
 */
void files_reducer(char* temp_file, char* output_file) {
    FILE* temp_f = fopen(temp_file, "r");
    char* buffer_line = NULL;

    if (!temp_f){
        perror("Cannot open temp_file");
        exit(EXIT_FAILURE);
    }

    sender_t* temp_linked_list = NULL;
    size_t buffer_size = 0;
    while (getline(&buffer_line, &buffer_size, temp_f) != EOF){
        if (buffer_size <= 120) {
            buffer_size = strlen(buffer_line)+1;
        }
        buffer_line[buffer_size-1] = '\0';

        char* newline;
        while ((newline = strchr(buffer_line, '\n')) != NULL) {
            *newline = '\0';
        }

	      char* piece = strtok(buffer_line, " ");
        char sender[STR_MAX_LEN];
        strcpy(sender, piece);
        temp_linked_list = add_source_to_list(temp_linked_list, sender);
        
        sender_t *source = find_source_in_list(temp_linked_list, sender);
        while ((piece = strtok(NULL, " "))) {
            add_recipient_to_source(source, piece);
        }
        
        buffer_size = 0; // reset buffer size so getline will allocate a new buffer & not use the old one & economize memory
        free(buffer_line); // free the old buffer
    }
    free(buffer_line);

    fclose(temp_f);

    FILE* output = fopen(output_file, "w");
    if (!output){
        perror("Cannot open output_file");
        exit(EXIT_FAILURE);
    }
    
    sender_t* temp_sender = temp_linked_list;
    recipient_t* temp_recipient;

    while (temp_sender != NULL) {
        temp_recipient = temp_sender->head;
        fprintf(output, "%s ", temp_sender->sender_address);

        while (temp_recipient != NULL) {
            fprintf(output, "%d:%s ", temp_recipient->occurrences, temp_recipient->recipient_address);
            temp_recipient = temp_recipient->next;
        }
        fprintf(output, "\n");
        temp_sender = temp_sender->next;
    }


    fclose(output);
    clear_sources_list(temp_linked_list);
}
