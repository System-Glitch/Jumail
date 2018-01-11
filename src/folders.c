/**
 * 	folders.c
 *
 *  Author : Jérémy LAMBERT
 *  Date : 28/12/2017
 *  Description : Mailboxes and folders utilities
 */
#include "folders.h"
#include "libs/muttx/utf7.h"

/**
 * Parses a line returned from a LIST (IMAP) command using regex to get the folder name
 */
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

/**
 * Parses the whole result of a LIST (IMAP) command to get the folder names and returns the result into a StringArray.
 * The returned StringArray MUST be freed using free_string_array()
 */
static StringArray *parse_list(CURL *curl, char * list) {
	char * tmp; //Raw list line
	char * tmp2; //Unescaped line
	char * tmp3; //UTF-7 decoded
	int j = 0;
	int len;
	StringArray *result;

	result = malloc(sizeof(StringArray));
	if(result == NULL) {
		fputs("Error when allocating parsed list array.\n", stderr);
		return NULL;
	}

	StringArray array = split_list(list); //Split the result by lines in order to process each line individually

	result->size = array.size;
	result->array = malloc(sizeof(char*)*result->size);
	if(result->array == NULL) {
		free_string_array(array);
		fputs("Error when allocating parsed list array.\n", stderr);
		return NULL;
	}

	for(size_t i = 0 ; i < array.size ; i++) { //Individually process each line
		tmp = parse_list_line(array.array[i]);
		if(tmp == NULL) {
			free_string_array(*result);
			free_string_array(array);
			fputs("Couldn't extract folder name.\n", stderr);
			return NULL;
		}

		tmp2 = curl_easy_unescape(curl, tmp, 0, &len);
		free(tmp);

		utf7_to_utf8(tmp2, strlen(tmp2), &tmp3, 0); //Decode special chars
		curl_free(tmp2);

		result->array[j] = malloc(len+1);
		if(result->array[j] == NULL) {
			free_string_array(*result);
			free_string_array(array);
			fputs("Error when allocating decoded folder name.\n", stderr);
			return NULL;
		}
		strcpy(result->array[j], tmp3);

		free(tmp3);
		j++;
	}

	free_string_array(array);
	return result;
}

/**
 * Performs a LIST (IMAP) operation, parses it and returns the existing folders inside a StringArray
 */
StringArray *ssl_list(char * username, char * password, char * domain, char ssl) {
	CURL *curl;
	CURLcode res = CURLE_OK;
	StringArray *list = NULL;
	struct MemoryStruct chunk;
	char * address;

	address = generate_address(domain, ssl ? "imaps":"imap");
	if(address == NULL) {
		fprintf(stderr, "Error while creating IMAP address from domain.\n");
		return NULL;
	}

	chunk.memory = malloc(1);
	chunk.size = 0;

	curl = curl_easy_init();
	if(curl) {
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); //Toggle full logging
		curl_easy_setopt(curl, CURLOPT_PROTOCOLS, CURLPROTO_IMAPS);
#if defined(_WIN32)
		curl_easy_setopt(curl, CURLOPT_CAINFO, "ssl/certs/ca-bundle.crt");
#endif
		curl_easy_setopt(curl, CURLOPT_USERNAME, username);
		curl_easy_setopt(curl, CURLOPT_PASSWORD, password);
		curl_easy_setopt(curl, CURLOPT_URL,address);

		//enable_tls(curl);

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

/**
 * Performs a folder creation or folder deletion.
 * action  = 1 -> CREATE
 * action != 1 -> DELETE
 */
static int ssl_folder(char * username, char * password, char * domain, char * mailbox, char ssl, int action) {
	CURL *curl;
	CURLcode res = CURLE_OK;
	int mailboxlen = 0;
	char * address;
	char * full_request;
	char * mailbox_encoded;

	address = generate_address(domain, ssl ? "imaps":"imap");
	if(address == NULL) {
		fprintf(stderr, "Error while creating IMAP address from domain.\n");
		return -1;
	}
	if(mailbox == NULL) {
		fprintf(stderr, "mailbox is not nullable.\n");
		return -1;
	}

	curl = curl_easy_init();
	if(!curl) {
		fprintf(stderr, "Error on creating curl.\n");
		return -1;
	}
	mailbox_encoded = url_encode(curl, mailbox);
	if(mailbox_encoded == NULL) {
		fprintf(stderr, "Error on URL encoding.\n");
		curl_easy_cleanup(curl);
		return -1;
	}

	mailboxlen = strlen(mailbox_encoded);

	full_request = malloc(7+mailboxlen+2+1);
	if(full_request == NULL) {
		fprintf(stderr, "Error while creating IMAP request.\n");
		curl_easy_cleanup(curl);
		return -1;
	}

	strcpy(full_request, action ? "CREATE \"" : "DELETE \"");
	strcat(full_request, mailbox_encoded);
	strcat(full_request, "\"");
	free(mailbox_encoded);

	if(curl) {
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); //Toggle full logging
		curl_easy_setopt(curl, CURLOPT_PROTOCOLS, CURLPROTO_IMAPS);
#if defined(_WIN32)
		curl_easy_setopt(curl, CURLOPT_CAINFO, "ssl/certs/ca-bundle.crt");
#endif
		curl_easy_setopt(curl, CURLOPT_USERNAME, username);
		curl_easy_setopt(curl, CURLOPT_PASSWORD, password);

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_null); //Don't print the result to stdout

		curl_easy_setopt(curl, CURLOPT_URL,address);
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST,full_request);

		//enable_tls(curl);

		curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

		res = curl_easy_perform(curl);

		/* Check for errors */
		if(res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
					curl_easy_strerror(res));
		else {
			fputs("Folder ", stdout);
			fputs(action ? "created.\n" : "deleted.\n", stdout);
		}

		/* Always cleanup */
		curl_easy_cleanup(curl);
	}
	free(address);
	free(full_request);

	return (int)res;
}

/**
 * Performs a CREATE (IMAP) operation in order to create a new mailbox
 */
int ssl_create_folder(char * username, char * password, char * domain, char * mailbox, char ssl) {
	return ssl_folder(username, password, domain, mailbox, ssl, 1);
}

/**
 * Performs a DELETE (IMAP) operation in order to delete an existing mailbox
 */
int ssl_remove_folder(char * username, char * password, char * domain, char * mailbox, char ssl) {
	return ssl_folder(username, password, domain, mailbox, ssl, 0);
}

int parse_folder_size(char *payload) {
	char *sub = NULL;
	regex_t regex;
	regmatch_t pmatch[3];
	int len;
	int res;

	if(payload == NULL || strlen(payload) <= 0)
		return -1;

	if(exec_regex(&regex, REGEX_SIZE, payload, 3, &pmatch)) {
		len = pmatch[2].rm_eo - pmatch[2].rm_so;
		sub = malloc(len + 1);
		if(sub == NULL) {
			regfree(&regex);
			return -1;
		}
		strncpy(sub, payload+pmatch[2].rm_so, len);
		sub[len] = '\0';
	}

	regfree(&regex);

	res = strtol(sub, NULL, 10);
	free(sub);

	return res;
}

