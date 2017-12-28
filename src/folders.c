/**
 * 	folders.c
 *
 *  Author : Jérémy LAMBERT
 *  Date : 28/12/2017
 *  Description : Mailboxes and folders utilities
 */
#include "folders.h"

static char * parse_list_line(char* line) {
	regex_t regex;
	regmatch_t pmatch[3];
	int len;
	char * dest = NULL;

	if(exec_regex(&regex, REGEX_FOLDER, line, 3, &pmatch)) {
		len = pmatch[2].rm_eo - pmatch[2].rm_so;
		dest = malloc(len + 1);
		if(dest == NULL) {
			regfree(&regex);
			return NULL;
		}
		strncpy(dest, line+pmatch[2].rm_so, len);
		dest[len] = '\0';
	}

	regfree(&regex);
	return dest;
}

static StringArray *parse_list(CURL *curl, char * list) {
	char * tmp;
	char * tmp2;
	int j = 0;
	int len;
	StringArray *result;

	result = malloc(sizeof(StringArray));
	if(result == NULL) {
		fputs("Error when allocating parsed list array.\n", stderr);
		return NULL;
	}

	StringArray array = split_list(list);

	result->size = array.size;
	result->array = malloc(sizeof(char*)*result->size);
	if(result->array == NULL) {
		free_string_array(array);
		fputs("Error when allocating parsed list array.\n", stderr);
		return NULL;
	}

	for(int i = 0 ; i < array.size ; i++) {
		tmp = parse_list_line(array.array[i]);
		if(tmp == NULL) {
			free_string_array(*result);
			free_string_array(array);
			fputs("Couldn't extract folder name.\n", stderr);
			return NULL;
		}

		tmp2 = curl_easy_unescape(curl, tmp, 0, &len);
		free(tmp);

		result->array[j] = malloc(len+1);
		if(result->array[j] == NULL) {
			free_string_array(*result);
			free_string_array(array);
			fputs("Error when allocating decoded folder name.\n", stderr);
			return NULL;
		}
		strcpy(result->array[j], tmp2);

		curl_free(tmp2);
		j++;
	}

	free_string_array(array);
	return result;
}

StringArray *ssl_list(char * username, char * password, char * domain) {
	CURL *curl;
	CURLcode res = CURLE_OK;
	StringArray *list = NULL;
	struct MemoryStruct chunk;
	char * address;

	address = generate_address(domain, "imaps");
	if(address == NULL) {
		fprintf(stderr, "Error while creating IMAP address from domain.\n");
		return NULL;
	}

	chunk.memory = malloc(1);
	chunk.size = 0;

	curl = curl_easy_init();
	if(curl) {
		curl_easy_setopt(curl, CURLOPT_PROTOCOLS, CURLPROTO_IMAPS);
		curl_easy_setopt(curl, CURLOPT_USERNAME, username);
		curl_easy_setopt(curl, CURLOPT_PASSWORD, password);
		curl_easy_setopt(curl, CURLOPT_URL,address);

		enable_ssl(curl);

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_memory_callback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

		res = curl_easy_perform(curl);

		/* Check for errors */
		if(res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
					curl_easy_strerror(res));
		else {
			list = parse_list(curl, chunk.memory);
		}

		free(chunk.memory);
		/* Always cleanup */
		curl_easy_cleanup(curl);
	}
	free(address);

	return list;
}

int ssl_create_folder(char * username, char * password, char * domain, char * mailbox) {
	return 0;
}

int ssl_remove_folder(char * username, char * password, char * domain, char * mailbox) {
	return 0;
}

