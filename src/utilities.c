/**
 * 	utilities.c
 *
 *  Author : Jérémy LAMBERT
 *  Date : 27/11/2017
 *  Description : General purpose utilities
 */
#include "utilities.h"
#include <stdio.h>

void free_string_array(StringArray arr) {
	for(int i  = 0 ; i < arr.size ; i++)
		free(arr.array[i]);
}

StringArray strsplit(char * str , char delimiter) {
	StringArray array;
	int next_delimiter;
	int count, len, index, j;

	array.size = -1;
	array.array = NULL;
	count = strcount(str, delimiter);
	len = strlen(str);

	if(count == 0) { //Nothing to split, return a copy of the string
		array.array = malloc(sizeof(char *));
		if(array.array == NULL) return array;
		array.array[0] = malloc(len+1);
		if(array.array[0] == NULL) return array;
		strcpy(array.array[0], str);
		array.size = 1;
	} else {
		array.array = malloc((count+1)*sizeof(char *));
		if(array.array == NULL) return array;
		array.size = count+1;

		next_delimiter = j = index = 0;

		while(j <= len) {
			for(int i = j ; i < len ; i++) { //Find the next delimiter
				next_delimiter = i;
				if(str[i] == delimiter) {
					break;
				}
			}

			if(next_delimiter == j-1) { //The end of the string is the delimiter
				array.array[index] = malloc(1);
				if(array.array[index] == NULL) {
					array.size = -1;
					return array;
				}
				array.array[index][0] = '\0'; //Give an empty string then
				return array;
			}

			if(next_delimiter == len-1 && str[next_delimiter] != delimiter) next_delimiter++; //The end of the string is NOT the delimiter, add 1 so the last delimiter is '\0'

			array.array[index] = malloc(next_delimiter - j + 1);
			if(array.array[index] == NULL) {
				array.size = -1;
				return array;
			}

			strncpy(array.array[index] , str+j, next_delimiter-j);
			array.array[index][next_delimiter-j] = '\0';
			index++;
			j = next_delimiter+1; //Skip the delimiter for next part
		}
	}
	return array;
}

StringArray split_mail(char * mail) {
	StringArray array;
	int next_delimiter;
	int len, index, j;
	int outsideHeader = 0;
	char * next;

	array.size = -1;
	array.array = NULL;
	len = strlen(mail);

	array.array = malloc(1*sizeof(char*));
	if(array.array == NULL) return array;

	j = index = 0;
	while(j < len) {
		next_delimiter = -1;
		if(!outsideHeader) //Ignore the delimiter outside the header
			for(int i = j ; i < len-1 ; i++) { //Find the next delimiter
				if(mail[i] == '\r' && mail[i+1] == '\n') {
					next_delimiter = i;
					break;
				}
			}

		if(next_delimiter == -1) { //Ignore the delimiters in the message body
			j+= 2; //Skip the divider, see RFC5322
			array.array = realloc(array.array, (index+1)*sizeof(char*));
			array.array[index] = malloc(len - j + 1);
			if(array.array[index] == NULL) {
				array.size = -1;
				return array;
			}
			if(len - j > 0) //Check that the message is not empty
				strcpy(array.array[index], mail+j);
			else
				array.array[index][0] = '\0'; //If the message is empty, give an empty string
			array.size = index+1;
			return array;
		}

		array.array = realloc(array.array, (index+1)*sizeof(char*));
		array.array[index] = malloc(next_delimiter - j + 1);
		if(array.array[index] == NULL) {
			array.size = -1;
			return array;
		}

		strncpy(array.array[index] , mail+j, next_delimiter-j);
		array.array[index][next_delimiter-j] = '\0';
		index++;
		array.size = index;
		j = next_delimiter+2; //Skip the delimiter "\r\n"

		next = malloc(3);
		strncpy(next,mail+j, 2);
		next[2] = '\0';
		if(!strcmp(next,"\r\n")) { //Check if end of header was reached
			outsideHeader = 1;
		}
		free(next);
	}

	return array;
}

int strcount(char * str, char c) {
	int i,count,len;

	count = 0;

	if(str == NULL) return count;
	len = strlen(str);
	for (i=0; i < len; i++)
		if(str[i] == c)
			count++;

	return count;
}


