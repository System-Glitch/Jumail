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

typedef struct StringArray {
	size_t  size;
	char ** array;
} StringArray;

void free_string_array(StringArray arr);

/**
 * Splits the given string according to the delimiter.
 */
StringArray strsplit(char * str , char delimiter);

/**
 * Counts the amount of the char c in the string str
 */
int strcount(char * str, char c);

#endif /* SRC_UTILITIES_H_ */
