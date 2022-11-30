//
// Created by flassabe on 26/10/22.
//

#include "reducers.h"

#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "global_defs.h"
#include "utility.h"

/*!
 * @brief add_source_to_list adds an e-mail to the sources list. If the e-mail already exists, do not add it.
 * @param list the list to update
 * @param source_email the e-mail to add as a string
 * @return a pointer to the updated beginning of the list
 */
sender_t* add_source_to_list(sender_t* list, char* source_email)
{   
    //marche pas
    // sender_t* temp_sender = list;
    // bool is_here = false;

    // while (temp_sender != NULL && is_here == false) {
    //     if (strcmp(temp_sender->sender_address, source_email) == 0) {
    //         is_here = true;
    //         return list;
    //     }
    //     temp_sender = temp_sender->next;
    // }
    // sender_t* new_sender = (sender_t*)malloc(sizeof(sender_t));
    // strncpy(new_sender->sender_address,source_email,STR_MAX_LEN);

    // if(list != NULL){ 
    //     new_sender->next = list;
    //     list->prev = new_sender;
    // }else{
    //     new_sender->next = NULL;
    //     new_sender->prev = NULL;
    // }   

    // new_sender->head = NULL;
    // new_sender->tail = NULL;

    // return new_sender;
}

/*!
 * @brief clear_sources_list clears the list of e-mail sources (therefore clearing the recipients of each source)
 * @param list a pointer to the list to clear
 */
void clear_sources_list(sender_t* list)
{   
    //marche pas non plus
    // if (list == NULL) {
    //     return;
    // } else {
    //     clear_sources_list(list->next);
    //     while (list->head->next != NULL) {
    //         list->head = list->head->next;
    //         free(list->head->prev);
    //     }
    //     free(list->head);
    //     free(list);
    // }
}

/*!
 * @brief find_source_in_list looks for an e-mail address in the sources list and returns a pointer to it.
 * @param list the list to look into for the e-mail
 * @param source_email the e-mail as a string to look for
 * @return a pointer to the matching source, NULL if none exists
 */
sender_t* find_source_in_list(sender_t* list, char* source_email)
{
    sender_t* temp = list;

    while (temp != NULL && temp->sender_address != source_email) {
        temp = temp->next;
    }
    if (temp == NULL) {
        return NULL;
    } else {
        return temp;
    }
}

/*!
 * @brief add_recipient_to_source adds or updates a recipient in the recipients list of a source. It looks for
 * the e-mail in the recipients list: if it is found, its occurrences is incremented, else a new recipient is created
 * with its occurrences = to 1.
 * @param source a pointer to the source to add/update the recipient to
 * @param recipient_email the recipient e-mail to add/update as a string
 */
void add_recipient_to_source(sender_t* source, char* recipient_email)
{
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
}

/*!
 * @brief files_reducer opens the second temporary output file (default step2_output) and collates all sender/recipient
 * information as defined in the project instructions. Stores data in a double level linked list (list of source e-mails
 * containing each a list of recipients with their occurrences).
 * @param temp_file path to temp output file
 * @param output_file final output file to be written by your function
 */
void files_reducer(char* temp_file, char* output_file)
{
}
