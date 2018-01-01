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
#define REGEX_MESSAGE_ID	"Message-ID: (.*)"
#define REGEX_IN_REPLY_TO	"In-Reply-To: (.*)"
#define REGEX_REFERENCES	"References: (.*)"
#define REGEX_FLAGS			"\\* (.*?) FETCH \\(FLAGS \\((.*?)\\)\\)"

struct ParsedSearch {
	size_t size;
	int * uids;
};

typedef struct Email {
	char 		*date;
	char 		*to;
	char 		*from;
	char 		*subject;
	char 		*message;
	char 		*message_id;
	char 		*in_reply_to;
	char 		*references;
	StringArray *flags;
	char		*raw;
} Email;

/**
 * Generates a GUID using an online service.
 * Don't forget to free it.
 */
char * generate_id();

/**
 * Generates the full mail header. "name", "in_reply_to" and "references" are nullable. Returns NULL if an error occurred or some inputs are wrong.
 * In case of a reply, if the parent mail doesn't have a References header, pass the value of the In-Reply-To header in the "references" field. See RFC 2822
 * Don't forget to free it.
 */
char ** get_header(char *from, char *to, char *name, char *subject, char *in_reply_to, char *references, char *id);

/**
 * Generates the whole message payload. Returns NULL if an error occurred or if invalid parameters are given.
 * Don't forget to free it.
 */
char ** get_mail(char ** header, char * message);


void free_header(char ** header);
void free_mail(char ** header);

/**
 * Sends a mail generated with get_mail using SMTP. "mail" is the full email payload, built with get_mai() and get_header()
 */
int send_mail_ssl(char * username, char * password, char * to, char * domain, const char ** mail);

/**
 * Performs a SEARCH ?ALL (IMAP) operation in the given mailbox. This returns all the UIDs present in the mailbox.
 */
struct ParsedSearch *ssl_search_all(char * username, char * password, char * domain, char * mailbox);

/**
 * Performs a FETCH (IMAP) operation to get an email. Returns NULL if an error occurred
 * Don't forget to free it with free_email()
 */
Email *ssl_get_mail(char * username, char * password, char * domain, char * mailbox, int uid);

/**
 * Performs a STORE operation (IMAP). The request is a formatted string.
 * Example: "STORE %d +Flags \\Deleted"
 * Here, %d will be replaced by uid
 */
int ssl_mail_request(char * username, char * password, char * domain, char * mailbox, char *message_id, const char *request);

/**
 * Performs a STORE (IMAP) operation to flag an email as seen (1) or unseen (0).
 */
int ssl_see_mail(char * username, char * password, char * domain, char * mailbox, char *message_id, char seen);

/**
 * Performs a STORE (IMAP) operation to flag an email as deleted.
 */
int ssl_delete_mail(char * username, char * password, char * domain, char * mailbox, char *message_id);

/**
 * Parses a complete email payload (header + body) and returns the result into an Email struct
 */
Email *parse_email(char * payload);

/**
 * Safe free of an Email struct, ignoring NULL pointers
 */
void free_email(Email *email);

/**
 * Moves an email from one folder to another performing a COPY (IMAP) operation then flags the mail as deleted in the source folder
 */
int ssl_move_mail(char * username, char * password, char * domain, char * mailbox_src, char * mailbox_dst, char *message_id);

/**
 * Searches an email by Message-ID and returns the UID if found
 * Uses an already existing CURL connection
 * Returns 0 if not found, -1 if an error occurred
 */
int ssl_search_by_id(CURL *curl, char *message_id);

/**
 * Safe free of a ParsedSearch struct, ignoring NULL pointers
 */
void free_parsed_search(struct ParsedSearch *search);

#endif /* SRC_MAILING_H_ */
