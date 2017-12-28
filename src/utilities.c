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
	if(arr.array != NULL)
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

StringArray split_list(char * list) {
	StringArray array;
	int next_delimiter;
	int len, index, j;
	int outsideHeader = 0;

	array.size = -1;
	array.array = NULL;
	len = strlen(list);

	array.array = malloc(1*sizeof(char*));
	if(array.array == NULL) return array;

	j = index = 0;
	while(j < len) {
		next_delimiter = -1;
		if(!outsideHeader) //Ignore the delimiter outside the header
			for(int i = j ; i < len-1 ; i++) { //Find the next delimiter
				if(list[i] == '\r' && list[i+1] == '\n') {
					next_delimiter = i;
					break;
				}
			}

		array.array = realloc(array.array, (index+1)*sizeof(char*));
		array.array[index] = malloc(next_delimiter - j + 1);
		if(array.array[index] == NULL) {
			array.size = -1;
			return array;
		}

		strncpy(array.array[index] , list+j, next_delimiter-j);
		array.array[index][next_delimiter-j] = '\0';
		index++;
		array.size = index;
		j = next_delimiter+2; //Skip the delimiter "\r\n"
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

char * url_encode(CURL *curl, char * str) {
	char * result = NULL;
	char * tmp;
	int sum = 0;

	result = malloc(strlen(str));
	if(result == NULL)
		return result;

	result[0] = '\0';

	StringArray arr = strsplit(str, '/');

	for(int i = 0 ; i < arr.size ; i++) {
		tmp = curl_easy_escape(curl, arr.array[i], 0);
		sum += strlen(tmp)+1;

		result = realloc(result, sum+1);
		if(result == NULL)
			return result;

		strcat(result,tmp);

		if(i != arr.size-1)
			strcat(result,"/");

		curl_free(tmp);
	}

	return result;
}

size_t write_to_null(void *contents, size_t size, size_t nmemb, void *userp) { return nmemb; }

size_t write_memory_callback(void *contents, size_t size, size_t nmemb, void *userp) {
	size_t realsize = size * nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct *)userp;

	mem->memory = realloc(mem->memory, mem->size + realsize + 1);
	if(mem->memory == NULL) {
		/* out of memory! */
		printf("not enough memory (realloc returned NULL)\n");
		return 0;
	}

	memcpy(&(mem->memory[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;

	return realsize;
}

size_t payload_source(void *ptr, size_t size, size_t nmemb, void *userp) {
	struct upload_status *upload_ctx = (struct upload_status *)userp;
	const char *data;

	if((size == 0) || (nmemb == 0) || ((size*nmemb) < 1)) {
		return 0;
	}

	data = upload_ctx->content[upload_ctx->lines_read];

	if(data) {
		size_t len = strlen(data);
		memcpy(ptr, data, len);
		upload_ctx->lines_read++;

		return len;
	}

	return 0;
}


void enable_ssl(CURL *curl) {
	#ifdef SKIP_PEER_VERIFICATION
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	#endif

	#ifdef SKIP_HOSTNAME_VERIFICATION
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
	#endif
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
}

int exec_regex(regex_t * regex, char* regexp, char * source, int max_groups, regmatch_t (*pmatch)[]) {
	if (regcomp(regex, regexp, REG_EXTENDED)) {
		fputs("Could not compile regular expression.\n", stderr);
		return 0;
	}

	return regexec(regex, source, max_groups, *pmatch, 0) == 0;
}

char * generate_address(char * domain, char * protocol) {
	char * address;
	int domainlen, protocollen;

	if(domain == NULL || protocol == NULL) return NULL;
	domainlen = strlen(domain);
	protocollen = strlen(protocol);

	address = malloc(domainlen + protocollen +3); // +3 for "://", +2 for "/\0"
	if(address == NULL) return NULL;

	strcpy(address, protocol);
	strcat(address, "://");
	strcat(address, domain);
	strcat(address, "/");

	return address;
}
