//
// Created by flassabe on 26/10/22.
//

#include "analysis.h"

#include <dirent.h>
#include <stddef.h>
#include <unistd.h>
#include <libgen.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/file.h>

#include "utility.h"

/*!
 * @brief parse_dir parses a directory to find all files in it and its subdirs (recursive analysis of root directory)
 * All files must be output with their full path into the output file.
 * @param path the path to the object directory
 * @param output_file a pointer to an already opened file
 */
void parse_dir(char *path, FILE *output_file) {
    // 1. Check parameters

    //if (!directory_exists(path)) return; TODO:
    DIR *dir = opendir(path);
    if (!dir) return;
    struct dirent *entries = readdir(dir);

    char *entry_path = (char *) malloc(sizeof(char) * STR_MAX_LEN);

    // 2. Gor through all entries: if file, write it to the output file; if a dir, call parse dir on it
    while (entries) {
        entries = next_dir(entries, dir);
        if (entries) {
        switch (entries->d_type) {
            case DT_DIR:
                entry_path = concat_path(path, entries->d_name, entry_path);
                parse_dir(entry_path, output_file);
                break;
            case DT_REG:
                entry_path = concat_path(path, entries->d_name, entry_path);
                fprintf(output_file, "%s\n", entry_path);
                break;
            default:
                break;
        }

        }
    }
    // 3. Clear all allocated resources (dirent pointer should not be free'd)
    closedir(dir);
    free(entry_path);
}

/*!
 * @brief clear_recipient_list clears all recipients in a recipients list
 * @param list the list to be cleared
 */
void clear_recipient_list(simple_recipient_t *list) {
    if (list) {
        clear_recipient_list(list->next);
        free(list);
    }
}

/*!
 * @brief add_recipient_to_list adds a recipient to a recipients list (as a pointer to a recipient)
 * @param recipient_email the string containing the e-mail to add
 * @param list the list to add the e-mail to
 * @return a pointer to the new recipient (to update the list with)
 */
simple_recipient_t *add_recipient_to_list(char *recipient_email, simple_recipient_t *list) {
    simple_recipient_t *new_mail = (simple_recipient_t *) malloc(sizeof(simple_recipient_t));
    strcpy(new_mail->email, recipient_email);
    new_mail->next =  list;
    return new_mail;
}


/*!
 * @brief extract_e_mail extracts an e-mail from a buffer
 * @param buffer the buffer containing the e-mail
 * @param destination the buffer into which the e-mail is copied
 */
void extract_e_mail(char buffer[], char destination[]) {
    sscanf(buffer, "%[^\n]", destination); //getting rid of \n
    char *last = strrchr(destination, ' '); //finding last word
    if (last) strcpy(destination, last);
    sscanf(destination, "%s", destination); //getting rid of tabs and spaces
}

/*!
 * @brief extract_emails extracts all the e-mails from a buffer and put the e-mails into a recipients list
 * @param buffer the buffer containing one or more e-mails
 * @param list the resulting list
 * @return the updated list
 */
simple_recipient_t *extract_emails(char *buffer, simple_recipient_t *list) {
    if (buffer) { // 1. Check parameters
        char *token = (char *) malloc(sizeof(char) * STR_MAX_LEN);
        token = strtok(buffer, ",");
        while (token) {
            char dest[STR_MAX_LEN];
            extract_e_mail(token, dest); // 2. Go through buffer and extract e-mails
            if (strlen(dest) > 2) list = add_recipient_to_list(dest, list); // 3. Add each e-mail to list
            token = strtok(NULL, ",");
        }
        free(token);
    }
    return list; // 4. Return list
}

// Used to track status in e-mail (for multi lines To, Cc, and Bcc fields)
typedef enum {IN_DEST_FIELD, OUT_OF_DEST_FIELD} read_status_t;

/*!
 * @brief parse_file parses mail file at filepath location and writes the result to
 * file whose location is on path output
 * @param filepath name of the e-mail file to analyze
 * @param output path to output file
 * Uses previous utility functions: extract_email, extract_emails, add_recipient_to_list,
 * and clear_recipient_list
 */
void parse_file(char *filepath, char *output) {
    FILE *file, *output_file;

    if (!(file = fopen(filepath, "r")) || !(output_file = fopen(output, "a"))) {
        return; // 1. Check parameters
    }
    char *buffer = (char *) malloc(sizeof(char) * STR_MAX_LEN);
    char sender[STR_MAX_LEN];
    read_status_t read_status = OUT_OF_DEST_FIELD;
    simple_recipient_t *recipient_list = NULL;

    // 2. Go through e-mail and extract From: address into a buffer
    while ((buffer = fgets(buffer,STR_MAX_LEN, file)) && !strstr(buffer, "From:"));
    if (!buffer) {
        return;
    }
    extract_e_mail(buffer, sender);
    read_status = IN_DEST_FIELD;

    // 3. Extract recipients (To, Cc, Bcc fields) and put it to a recipients list.
    while (read_status == IN_DEST_FIELD) {
        fgets(buffer, STR_MAX_LEN, file);
        if (buffer[0] != '\t' && !strstr(buffer, "To:") && !strstr(buffer, "Cc:") && !strstr(buffer, "Bcc:")) {
            read_status = OUT_OF_DEST_FIELD;
        } else {
            recipient_list = extract_emails(buffer, recipient_list);
        }
    }

    flock(fileno(output_file), LOCK_EX); // 4. Lock output file

    // 5. Write to output file according to project instructions
    fprintf(output_file, "%s ", sender);
    for (simple_recipient_t *recipient = recipient_list; recipient != NULL; recipient = recipient->next) {
        fprintf(output_file, "%s ", recipient->email);
        printf("%s\n", recipient->email);
    }

    flock(fileno(output_file), LOCK_UN); // 6. Unlock file

    // 7. Close file
    fclose(file);
    fclose(output_file);

    // 8. Clear all allocated resources
    free(buffer);
    clear_recipient_list(recipient_list);
}

/*!
 * @brief process_directory goes recursively into directory pointed by its task parameter object_directory
 * and lists all of its files (with complete path) into the file defined by task parameter temporary_directory/name of
 * object directory
 * @param task the task to execute: it is a directory_task_t that shall be cast from task pointer
 * Use parse_dir.
 */
void process_directory(task_t *task) {
    // 1. Check parameters
    // 2. Go through dir tree and find all regular files
    // 3. Write all file names into output file
    // 4. Clear all allocated resources

}

/*!
 * @brief process_file processes one e-mail file.
 * @param task a file_task_t as a pointer to a task (you shall cast it to the proper type)
 * Uses parse_file
 */
void process_file(task_t *task) {
    // 1. Check parameters
    // 2. Build full path to all parameters
    // 3. Call parse_file
}
