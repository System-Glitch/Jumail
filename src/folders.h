/**
 * 	folders.h
 *
 *  Author : Jérémy LAMBERT
 *  Date : 28/12/2017
 *  Description : Mailboxes and folders utilities
 */

#ifndef SRC_FOLDERS_H_
#define SRC_FOLDERS_H_

#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>
#include "utilities.h"

#define REGEX_FOLDER		"\\* LIST \\((.*?)\\) \"\\/\" \"?([^\"]*)\"?"
#define REGEX_SIZE			"\\* STATUS (.*?) \\(MESSAGES (.*?)\\)"

/**
 * Performs a LIST (IMAP) operation, parses it and returns the existing folders inside a StringArray
 */
StringArray *ssl_list(char * username, char * password, char * domain, char ssl);

/**
 * Performs a CREATE (IMAP) operation in order to create a new mailbox
 */
int ssl_create_folder(char * username, char * password, char * domain, char * mailbox, char ssl);

/**
 * Performs a DELETE (IMAP) operation in order to delete an existing mailbox
 */
int ssl_remove_folder(char * username, char * password, char * domain, char * mailbox, char ssl);

int parse_folder_size(char *payload);

#endif /* SRC_FOLDERS_H_ */
