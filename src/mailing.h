/**
 * 	mailing.c
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
#include <regex.h>
#include "utilities.h"

#define REGEX_DATE 			"Date: (.*)"
#define REGEX_TO 			"To: (.*?)"
#define REGEX_FROM			"From: (.*?)"
#define REGEX_SUBJECT		"Subject: (.*)"

struct MemoryStruct {
  char *memory;
  size_t size;
};

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

char * generate_id();
char ** get_header(char * from, char * to, char * name, char * subject, char* id);
char ** get_mail(char ** header, char * message);
void free_header(char ** header);
void free_mail(char ** header);
int send_mail_ssl(char * username, char * password, char * to, char * domain, const char ** mail);
int examine_outbox_ssl(char * username, char * password);
int ssl_fetch(char * username, char * password, char * domain, char * mailbox);
int ssl_get_mail(char * username, char * password, char * domain, char * mailbox, int uid);
Email parse_email(char * payload, int uid);
void free_email(Email email);

#endif /* SRC_MAILING_H_ */
