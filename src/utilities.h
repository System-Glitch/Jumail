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

typedef struct StringArray {
	size_t  size;
	char ** array;
} StringArray;

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

#endif /* SRC_UTILITIES_H_ */
