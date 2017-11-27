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

	if(count == 0) {
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
			for(int i = j ; i < len ; i++) {
				next_delimiter = i;
				if(str[i] == delimiter) {
					break;
				}
			}

			if(next_delimiter == j-1) {
				array.array[index] = malloc(1);
				if(array.array[index] == NULL) {
					array.size = -1;
					return array;
				}
				array.array[index][0] = '\0';
				return array;
			}

			if(next_delimiter == len-1 && str[next_delimiter] != delimiter) next_delimiter++;

			array.array[index] = malloc(next_delimiter - j + 1);
			if(array.array[index] == NULL) {
				array.size = -1;
				return array;
			}

			strncpy(array.array[index] , str+j, next_delimiter-j);
			array.array[index][next_delimiter-j] = '\0';
			index++;
			j = next_delimiter+1;
		}
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


