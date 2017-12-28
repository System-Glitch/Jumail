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

#define REGEX_FOLDER		"\\* LIST \\((.*?)\\) \"\\/\" \"(.*?)\""

StringArray *ssl_list(char * username, char * password, char * domain);
int ssl_create_folder(char * username, char * password, char * domain, char * mailbox);
int ssl_remove_folder(char * username, char * password, char * domain, char * mailbox);

#endif /* SRC_FOLDERS_H_ */
