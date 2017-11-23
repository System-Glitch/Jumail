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

int ssl_fetch();
int ssl_append();

#endif /* SRC_MAILING_H_ */
