/**
 * 	mailing.h
 *
 *  Author : Jérémy LAMBERT
 *  Date : 23/11/2017
 *  Description : Mailing utilities
 */

#ifndef SRC_MAILING_H_
#define SRC_MAILING_H_

#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "utilities.h"

#define REGEX_DATE 			"Date: (.*)"
#define REGEX_TO 			"To: (.*?)"
#define REGEX_FROM			"From: (.*?)"
#define REGEX_SUBJECT		"Subject: (.*)"

struct ParsedSearch {
	size_t size;
	int * uids;
};

typedef struct Email {
	char * date;
	char * to;
	char * from;
	char * subject;
	char * message;
	int uid;
} Email;

/**
 * Generates a GUID using an online service.
 * Don't forget to free it.
 */
char * generate_id();

/**
 * Generates the full mail header. "name" is nullable. Returns NULL if an error occurred or some inputs are wrong.
 * Don't forget to free it.
 */
char ** get_header(char * from, char * to, char * name, char * subject, char* id);

/**
 * Generates the whole message payload. Returns NULL if an error occurred or if invalid parameters are given.
 * Don't forget to free it.
 */
char ** get_mail(char ** header, char * message);


void free_header(char ** header);
void free_mail(char ** header);

/**
 * Sends a mail generated with get_mail using SMTP.
 */
int send_mail_ssl(char * username, char * password, char * to, char * domain, const char ** mail);

/**
 * Performs a SEARCH ?ALL (IMAP) operation in the given mailbox. This returns all the UIDs present in the mailbox.
 */
int ssl_search_all(char * username, char * password, char * domain, char * mailbox);

/**
 * Performs a FETCH (IMAP) operation to get an email.
 */
int ssl_get_mail(char * username, char * password, char * domain, char * mailbox, int uid);

/**
 * Performs a STORE operation (IMAP). The request is a formatted string.
 * Example: "STORE %d +Flags \\Deleted"
 * Here, %d will be replaced by uid
 */
int ssl_mail_request(char * username, char * password, char * domain, char * mailbox, int uid, const char *request);

/**
 * Performs a STORE (IMAP) operation to flag an email as seen (1) or unseen (0).
 */
int ssl_see_mail(char * username, char * password, char * domain, char * mailbox, int uid, char seen);

/**
 * Performs a STORE (IMAP) operation to flag an email as deleted.
 */
int ssl_delete_mail(char * username, char * password, char * domain, char * mailbox, int uid);

/**
 * Parses a complete email payload (header + body) and returns the result into an Email struct
 */
Email parse_email(char * payload, int uid);

/**
 * Safe free of an Email struct, ignoring NULL pointers
 */
void free_email(Email email);

/**
 * Moves an email from one folder to another performing a COPY (IMAP) operation then flags the mail as deleted in the source folder
 */
int ssl_move_mail(char * username, char * password, char * domain, char * mailbox_src, char * mailbox_dst, int uid);

#endif /* SRC_MAILING_H_ */
