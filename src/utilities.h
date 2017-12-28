/**
 * 	utilities.h
 *
 *  Author : Jérémy LAMBERT
 *  Date : 27/11/2017
 *  Description : General purpose utilities
 */
#ifndef SRC_UTILITIES_H_
#define SRC_UTILITIES_H_

#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <regex.h>

struct upload_status {
	int lines_read;
	const char ** content;
};

typedef struct StringArray {
	size_t  size;
	char ** array;
} StringArray;

struct MemoryStruct {
  char *memory;
  size_t size;
};

void free_string_array(StringArray arr);

/**
 * Splits the given string according to the delimiter.
 * Don't forget to free_string_array !
 */
StringArray strsplit(char * str , char delimiter);

/**
 * Splits the given mail payload.
 * Don't forget to free_string_array !
 */
StringArray split_mail(char * mail);

/**
 * Splits the given list payload.
 * Don't forget to free_string_array !
 */
StringArray split_list(char * list);

/**
 * Counts the amount of the char c in the string str
 */
int strcount(char * str, char c);

/**
 * Splits the given string by slashes, URL encodes it, reassemble it and returns the result.
 * Don't forget to free it.
 */
char * url_encode(CURL *curl, char * str);

/**
 * Used by CURL to not write request result into memory
 */
size_t write_to_null(void *contents, size_t size, size_t nmemb, void *userp);

/**
 * Used by CURL to write request result into memory
 */
size_t write_memory_callback(void *contents, size_t size, size_t nmemb, void *userp);

size_t payload_source(void *ptr, size_t size, size_t nmemb, void *userp);

/**
 * Enables SSL on the given CURL connection
 */
void enable_ssl(CURL *curl);

/**
 * Executes the given regexp on the source and stores the result into the array pmatch
 */
int exec_regex(regex_t * regex, char* regexp, char * source, int max_groups, regmatch_t (*pmatch)[]);

/**
 * Generates the URL to the domain for the given protocol. Returns null in case of error or invalid string given.
 * Don't forget to free it.
 */
char * generate_address(char * domain, char * protocol);

#endif /* SRC_UTILITIES_H_ */
