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

#define FROM    "<jumailimap@gmail.com>"
#define TO      "<jumailimap@gmail.com>"

struct MemoryStruct {
  char *memory;
  size_t size;
};

char * generate_id();
char ** get_header(char * from, char * to, char * name, char * subject, char* id);
char ** get_mail(char ** header, char * message);
void free_header(char ** header);
void free_mail(char ** header);
int ssl_fetch();
int send_mail_ssl(char * username, char * password, char * to, char * domain, const char ** mail);
int examine_outbox_ssl(char * username, char * password);

#endif /* SRC_MAILING_H_ */
