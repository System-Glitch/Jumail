/**
 * 	mailing.c
 *
 *  Author : Jérémy LAMBERT
 *  Date : 23/11/2017
 *  Description : Mailing utilities
 */
#include "mailing.h"

linkedlist_t *loaded_mails = NULL;

void free_list_loaded_mails() {
	if(loaded_mails == NULL)
		return;

	if(loaded_mails->length){
		node_t * current = loaded_mails->head;
		while(current != NULL){
			free_email(current->val);
			current = current->next;
		}
	}
	linkedlist_free(loaded_mails);
}

/**
 * Generates a date at the correct format to directly include in the mail header.
 * Returns NULL in case of fail in memory allocation.
 * Don't forget to free it.
 */
static char * get_date() {
	char * date = malloc(80);
	if(date == NULL) return NULL;

	time_t now = time(NULL);
	struct tm *t = localtime(&now);

	strftime(date, 79, "Date: %a, %d %m %Y %H:%M:%S %z\r\n", t);
	return date;
}

/**
 * Generates the "TO" header line. Adds the chevrons.
 * Returns NULL in case of fail in memory allocation or if an invalid string was given.
 * Don't forget to free it.
 */
static char * get_to(char * to) {
	int tolen;
	char * result;

	if(to == NULL) return NULL;
	tolen = strlen(to)+1;
	if(tolen <= 0) return NULL;

	result = malloc(tolen+8); //+5 for "To: <", +3 for ">\r\n"
	if(result == NULL) return NULL;

	strcpy(result, "To: <");
	strcat(result, to);
	strcat(result, ">\r\n");
	return result;
}

/**
 * Generates the "FROM" header line. Adds the chevrons. "name" is nullable.
 * Returns NULL in case of fail in memory allocation or if an invalid string was given.
 * Don't forget to free it.
 */
static char * get_from(char * from, char* name) {
	int fromlen;
	int namelen = 0;
	char * result;

	if(from == NULL) return NULL;
	fromlen = strlen(from)+1;
	if(fromlen <= 0) return NULL;

	if(name != NULL)
		namelen = strlen(name)+2; //+2 for the parenthesis

	result = malloc(fromlen + namelen + 9); //+7 for "From: <", +2 for \r\n, +1 for \0
	if(result == NULL) return NULL;

	strcpy(result, "From: <");
	strcat(result, from);
	strcat(result, ">");

	if(name != NULL) {
		strcat(result, "(");
		strcat(result, name);
		strcat(result, ")");
	}
	strcat(result, "\r\n");
	return result;
}

/**
 * Generates the "Subject" header line.
 * Returns NULL in case of fail in memory allocation or if an invalid string was given.
 * Don't forget to free it.
 */
static char * get_subject(char * subject) {
	int tolen = -1;
	char * result;

	if(subject == NULL)
		tolen = 0;
	else
		tolen = strlen(subject);
	if(tolen < 0) return NULL;

	result = malloc(tolen+12); //+9 for "Subject: ", +2 for "\r\n", +1 for '\0'
	if(result == NULL) return NULL;

	strcpy(result, "Subject: ");
	if(subject == NULL)
		strcat(result, "");
	else
		strcat(result, subject);
	strcat(result, "\r\n");
	return result;
}

/**
 * Generates a GUID using an online service.
 * Don't forget to free it.
 */
char * generate_id() {
	CURL *curl_handle;
	CURLcode res;
	char * id = NULL;

	struct MemoryStruct chunk;

	chunk.memory = malloc(1);
	chunk.size = 0;

	curl_global_init(CURL_GLOBAL_ALL);
	curl_handle = curl_easy_init();
	curl_easy_setopt(curl_handle, CURLOPT_URL, "https://www.uuidgenerator.net/api/guid");
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_memory_callback);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
	res = curl_easy_perform(curl_handle);

	if(res != CURLE_OK) {
		fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));
	}
	else {
		id = chunk.memory;
		id[strlen(id)-2] = '\0'; //Remove \r\n
	}

	curl_easy_cleanup(curl_handle);
	curl_global_cleanup();

	return id;
}

/**
 * Generates  the "Message-ID" header line.
 * Returns NULL in case of fail in memory allocation or if an invalid string was given.
 */
static char * get_id(char * id) {
	int idlen;
	char * result;

	if(id == NULL) return NULL;
	idlen = strlen(id);
	if(idlen <= 0) return NULL;

	result = malloc(idlen+26+1); //+13 for "Message-ID: <", +13 for "@jumail.fr>\r\n", +1 for '\0'
	if(result == NULL) return NULL;

	strcpy(result, "Message-ID: <");
	strcat(result, id);
	strcat(result, "@jumail.fr>\r\n");
	return result;
}

/**
 * Generates  the "In-Reply-To" header line.
 * Returns NULL in case of fail in memory allocation or if an invalid string was given.
 */
static char * get_in_reply_to(char * id) {
	int idlen;
	char * result;

	if(id == NULL) return NULL;
	idlen = strlen(id);
	if(idlen <= 0) return NULL;

	result = malloc(idlen+13+2+1); //+13 for "In-Reply-To: ", +2 for "\r\n", +12 for "References: " +1 for '\0'
	if(result == NULL) return NULL;

	strcpy(result, "In-Reply-To: ");
	strcat(result, id);
	strcat(result, "\r\n");
	return result;
}

/**
 * Generates  the "References" header line. See RFC 2822
 * Returns NULL in case of fail in memory allocation or if an invalid string was given.
 */
static char * get_references(char *references, char *in_reply_to) {
	int referenceslen, in_reply_to_len;
	char * result;

	if(in_reply_to == NULL) return NULL;
	in_reply_to_len = strlen(in_reply_to);
	if(in_reply_to_len <= 0) return NULL;

	referenceslen = references != NULL ? strlen(references) : 0;
	if(referenceslen > 0) referenceslen++; //Already have references, need to add space separator to add in_reply_to after

	result = malloc(referenceslen+in_reply_to_len+13+2+1); //+13 for "In-Reply-To: ", +2 for "\r\n", +12 for "References: " +1 for '\0'
	if(result == NULL) return NULL;

	strcpy(result, "References: ");

	if(references != NULL) {
		strcat(result, references);
		strcat(result, " ");
	}
	strcat(result,in_reply_to);
	strcat(result, "\r\n");
	return result;
}

/**
 * Generates the full mail header. "name", "in_reply_to" and "references" are nullable. Returns NULL if an error occurred or some inputs are wrong.
 * In case of a reply, if the parent mail doesn't have a References header, pass the value of the In-Reply-To header in the "references" field. See RFC 2822
 * Don't forget to free it.
 */
char ** get_header(char *from, char *to, char *name, char *subject, char *in_reply_to, char *references, char *id) {
	char ** header;
	char * fromline = get_from(from, name);
	char * toline = get_to(to);
	char * dateline = get_date();
	char * idline = get_id(id);
	char * subjectline = get_subject(subject);
	char * in_reply_to_line = get_in_reply_to(in_reply_to);
	char * references_line = get_references(references, in_reply_to);
	char * separatorline;

	if(fromline == NULL || toline == NULL || dateline == NULL ||
			subjectline == NULL || idline == NULL) return NULL;

	separatorline = malloc(3);
	if(separatorline == NULL) return NULL;
	strcpy(separatorline, "\r\n");

	header = malloc(8*sizeof(char *));
	header[0] = dateline;
	header[1] = toline;
	header[2] = fromline;
	header[3] = idline;
	header[4] = subjectline;
	header[5] = in_reply_to_line;
	header[6] = references_line;
	header[7] = separatorline;

	return header;
}

void free_header(char ** header) {
	for(int i = 0 ; i < 8 ; i++)
		if(header[i] != NULL)
			free(header[i]);
	free(header);
}

/**
 * Generates the whole message payload. Returns NULL if an error occurred or if invalid parameters are given.
 * Don't forget to free it.
 */
char ** get_mail(char ** header, char * message) {
	char ** mail;
	int j = 0;

	if(header == NULL || message == NULL) return NULL;

	mail = malloc(10*sizeof(char *));
	if(mail == NULL) return NULL;

	for(int i = 0 ; i < 8 ; i++) { //Copy the header
		if(header[i] == NULL) continue;
		mail[j] = malloc(strlen(header[i])+1);
		if(mail[j] == NULL) return NULL;
		strcpy(mail[j], header[i]);
		j++;
	}
	mail[j] = malloc(strlen(message)+1);
	if(mail[j] == NULL) return NULL;
	strcpy(mail[j], message); //Copy the message

	j++;

	mail[j] = NULL; //End Delimiter

	for(int i = j ; i < 10 ; i++) {
		mail[i] = NULL; //Set all empty lines to NULL so no segfault when freeing
	}

	return mail;
}

void free_mail(char ** mail) {
	for(int i = 0 ; i < 10 ; i++)
		free(mail[i]);
	free(mail);
}

/**
 * Sends a mail generated with get_mail using SMTP. "mail" is the full email payload, built with get_mai() and get_header()
 */
int send_mail_ssl(char * username, char * password, char * to, char * domain, char ssl, char tls, const char ** mail) {
	CURL *curl;
	CURLcode res = CURLE_OK;
	struct curl_slist *recipients = NULL;
	const char **p;
	long infilesize;
	struct upload_status upload_ctx;
	char * address;

	address = generate_address(domain, ssl ? "smtps" : "smtp");
	if(address == NULL) {
		fprintf(stderr, "Error while creating SMTP address from domain.\n");
		return -1;
	}

	upload_ctx.lines_read = 0;
	upload_ctx.content = mail;

	curl = curl_easy_init();
	if(curl) {
		/* Set username and password */
		curl_easy_setopt(curl, CURLOPT_USERNAME, username);
		curl_easy_setopt(curl, CURLOPT_PASSWORD, password);

		curl_easy_setopt(curl, CURLOPT_URL,address);
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); //Toggle full logging

		if(tls)
			enable_tls(curl);

		curl_easy_setopt(curl, CURLOPT_MAIL_FROM, username); //Set sender

		recipients = curl_slist_append(recipients, to);
		curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

		curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source); //Set the source
		curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
		curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
		curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4); //Force ipv4

		infilesize = 0;
		for(p = mail; *p; ++p) {
			infilesize += (long)strlen(*p);
		}
		curl_easy_setopt(curl, CURLOPT_INFILESIZE, infilesize);

		res = curl_easy_perform(curl);

		/* Check for errors */
		if(res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

		/* Always cleanup */
		curl_easy_cleanup(curl);
		free(address);

	}

	return (int)res;
}

/**
 * Parses a String result of the SEARCH operation (IMAP) into an integer array containing all the UIDs.
 */
static struct ParsedSearch *parse_search(char * answer) {
	struct ParsedSearch *search;
	char * sub;
	char * end;
	int * result = NULL;
	int size, len, len2, i, index;

	search = malloc(sizeof(struct ParsedSearch));
	if(search == NULL) {
		fprintf(stderr, "Error while allocating for ParsedSearch.\n");
		return NULL;
	}

	search->uids = NULL;
	search->size = 0;

	size = strcount(answer, ' ') - 1;
	len = strlen(answer);
	len2 = len - 9; //Don't count unused characters for substring
	i = 0;

	if(size > 0) {
		result = malloc(size*sizeof(int));
		sub = malloc(len2);
		if(result == NULL || sub == NULL) {
			fprintf(stderr, "Memory allocation error when trying to parse FETCH result.\n");
			return search;
		}
		strcpy(sub, answer+9);

		end = sub;
		index = 0;
		while(*end) {
			int n = strtol(sub+index, &end, 10); //Parse uid to int
			result[i++] = n;
			while (*end == ' ') {
				end++;
			}
			if(*end == '\r' || *end == '\n') break; //Stop if delimiter is reached
			index = end - sub;
		}
		free(sub);
	} else {
		result = malloc(1);
	}

	search->uids = result;
	search->size = size;

	return search;
}


/**
 * Parses a line returned from a FETCH (FLAGS) (IMAP) command using regex to get the only the flags name
 */
static char * parse_flags_line(char* line, char *regexp) {
	regex_t regex;
	regmatch_t pmatch[3];
	int len;
	char * dest = NULL;

	if(exec_regex(&regex, regexp, line, 3, &pmatch)) {
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
 * Parses a string result of the FETCH (FLAGS) operation (IMAP) into a string array containing all the flags
 */
static StringArray *parse_flags(char * answer, char *regexp) {
	StringArray *flags;
	char *tmp;
	int len = 0;

	flags = malloc(sizeof(StringArray));
	if(flags == NULL) {
		fputs("Error when allocating parsed flags array.\n", stderr);
		return NULL;
	}

	//Extract only the flags using regex
	tmp = parse_flags_line(answer, regexp);
	if(tmp == NULL) {
		fputs("Error when extracting flags.\n", stderr);
		free(flags);
		return NULL;
	}

	StringArray array = strsplit(tmp, ' ');
	free(tmp);

	flags->size = array.size;
	flags->array = malloc(sizeof(char*)*flags->size);
	if(flags->array == NULL) {
		free_string_array(array);
		fputs("Error when allocating parsed list array.\n", stderr);
		return NULL;
	}

	for(size_t i = 0 ; i < array.size ; i++) { //Individually process each line
		len = strlen(array.array[i]);
		flags->array[i] = malloc(len+1);
		if(flags->array[i] == NULL) {
			free_string_array(*flags);
			free_string_array(array);
			fputs("Error when allocating decoded folder name.\n", stderr);
			return NULL;
		}
		strcpy(flags->array[i], array.array[i]);
	}

	free_string_array(array);
	return flags;

}

/**
 * Performs a SEARCH ?ALL (IMAP) operation in the given mailbox. This returns all the UIDs present in the mailbox.
 */
struct ParsedSearch *ssl_search_all(char * username, char * password, char * domain, char ssl, char * mailbox) {
	CURL *curl;
	CURLcode res = CURLE_OK;
	struct MemoryStruct chunk;
	struct ParsedSearch *search = NULL;
	int mailboxlen = 0;
	char * address;
	char * full_address;
	char * mailbox_encoded;

	address = generate_address(domain, ssl ? "imaps" : "imap");
	if(address == NULL) {
		fprintf(stderr, "Error while creating IMAP address from domain.\n");
		return NULL;
	}

	if(mailbox == NULL) {
		fprintf(stderr, "mailbox is not nullable.\n");
		free(address);
		return NULL;
	}

	curl = curl_easy_init();
	if(!curl) {
		fprintf(stderr, "Error on creating curl.\n");
		free(address);
		return NULL;
	}
	mailbox_encoded = url_encode(curl, mailbox);
	if(mailbox_encoded == NULL) {
		fprintf(stderr, "Error on URL encoding.\n");
		curl_easy_cleanup(curl);
		free(address);
		return NULL;
	}

	mailboxlen = strlen(mailbox_encoded);
	full_address = malloc(strlen(address)+mailboxlen+4+1);
	if(full_address == NULL) {
		fprintf(stderr, "Error while creating IMAP address from domain.\n");
		curl_easy_cleanup(curl);
		free(address);
		free(mailbox_encoded);
		return NULL;
	}

	//Building full address
	//imaps://domain/mailbox?ALL
	strcpy(full_address, address);
	free(address);
	strcat(full_address, mailbox_encoded);
	strcat(full_address, "?ALL");
	free(mailbox_encoded);

	chunk.memory = malloc(1); //Initial allocation. Will be reallocated in write_memory_callback() to fit the correct size.
	chunk.size = 0;

	if(curl) {
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); //Toggle full logging
		curl_easy_setopt(curl, CURLOPT_PROTOCOLS, CURLPROTO_IMAPS);
		curl_easy_setopt(curl, CURLOPT_USERNAME, username);
		curl_easy_setopt(curl, CURLOPT_PASSWORD, password);
		curl_easy_setopt(curl, CURLOPT_URL,full_address);

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
			search = parse_search(chunk.memory);
			if(search == NULL || search->uids == NULL) {
				fprintf(stderr, "Search is NULL\n");
				free(full_address);
				return NULL;
			}
		}

		free(chunk.memory);
		/* Always cleanup */
		curl_easy_cleanup(curl);
	}
	free(full_address);

	return search;
}

/**
 * Performs a FETCH (IMAP) operation to get an email. Returns NULL if an error occurred
 * Don't forget to free it with free_email()
 */
Email *ssl_get_mail(char * username, char * password, char * domain, char * mailbox, char ssl, int uid) {
	Email *mail = NULL;
	CURL *curl;
	CURLcode res = CURLE_OK;
	struct MemoryStruct chunk;
	int mailboxlen = 0, uidstrlen = 0;
	char * address;
	char * mailbox_encoded;
	char * full_address;
	char * full_request;
	char uidStr[12];

	address = generate_address(domain, ssl ? "imaps" : "imap");
	if(address == NULL) {
		fprintf(stderr, "Error while creating IMAP address from domain.\n");
		return NULL;
	}
	if(mailbox == NULL) {
		fprintf(stderr, "mailbox is not nullable.\n");
		return NULL;
	}

	sprintf(uidStr, "%d", uid);
	uidstrlen = strlen(uidStr);

	curl = curl_easy_init();
	if(!curl) {
		fprintf(stderr, "Error on creating curl.\n");
		return NULL;
	}
	mailbox_encoded = url_encode(curl, mailbox);
	if(mailbox_encoded == NULL) {
		fprintf(stderr, "Error on URL encoding.\n");
		curl_easy_cleanup(curl);
		return NULL;
	}
	mailboxlen = strlen(mailbox_encoded);

	full_address = malloc(strlen(address)+mailboxlen+uidstrlen+6+1);
	if(full_address == NULL) {
		fprintf(stderr, "Error while creating IMAP address from domain.\n");
		curl_easy_cleanup(curl);
		return NULL;
	}

	full_request = malloc(6+8+uidstrlen+1); //6 for "FETCH ", +8 for " (FLAGS)"
	if(full_request == NULL) {
		fprintf(stderr, "Error while creating IMAP address from domain.\n");
		curl_easy_cleanup(curl);
		return NULL;
	}

	//Building full address
	//imaps://domain/mailbox/;UID=uid
	strcpy(full_address, address);
	free(address);
	strcat(full_address, mailbox_encoded);
	strcat(full_address, "/;UID=");
	strcat(full_address, uidStr);


	//Building request
	strcpy(full_request,"FETCH ");
	strcat(full_request, uidStr);
	strcat(full_request, " (FLAGS)");

	free(mailbox_encoded);

	chunk.memory = malloc(1); //Initial allocation. Will be reallocated in write_memory_callback() to fit the correct size.
	chunk.size = 0;

	if(curl) {
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); //Toggle full logging
		curl_easy_setopt(curl, CURLOPT_PROTOCOLS, CURLPROTO_IMAPS);
		curl_easy_setopt(curl, CURLOPT_USERNAME, username);
		curl_easy_setopt(curl, CURLOPT_PASSWORD, password);

		curl_easy_setopt(curl, CURLOPT_URL,full_address);

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

			mail = parse_email(chunk.memory);
			if(mail == NULL) {
				fputs("Error, invalid mail payload.\n", stderr);
			} else {
				chunk.memory = malloc(1); //Initial allocation. Will be reallocated in write_memory_callback() to fit the correct size.
				chunk.size = 0;

				//Get flags
				curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST,full_request);

				res = curl_easy_perform(curl);

				/* Check for errors */
				if(res != CURLE_OK)
					fprintf(stderr, "curl_easy_perform() failed: %s\n",
							curl_easy_strerror(res));
				else {
					mail->flags = parse_flags(chunk.memory, REGEX_FLAGS);
				}

				mail->mailbox = malloc(strlen(mailbox)+1);
				if(mail->mailbox == NULL) {
					fputs("Couldn't allocate for mailbox copy.\n", stderr);
					free(chunk.memory);
					curl_easy_cleanup(curl);
					free(full_request);
					free(full_address);
					free_email(mail);
					free(mail);
					return NULL;
				} else {
					strcpy(mail->mailbox, mailbox);
				}

			}

			free(chunk.memory);
		}

		/* Always cleanup */
		curl_easy_cleanup(curl);
	}
	free(full_request);
	free(full_address);

	return mail;
}

/**
 * Performs a STORE operation (IMAP). The request is a formatted string.
 * Example: "STORE %d +Flags \\Deleted"
 * Here, %d will be replaced by uid
 */
int ssl_mail_request(char * username, char * password, char * domain, char * mailbox, char *message_id, char ssl, const char *request) {
	CURL *curl;
	CURLcode res = CURLE_OK;
	int mailboxlen = 0, uidstrlen = 0, requestlen = 0;
	char * address;
	char * full_address;
	char * full_request;
	char * mailbox_encoded;
	char uidStr[12];
	int uid;

	address = generate_address(domain, ssl ? "imaps" : "imap");
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
	full_address = malloc(strlen(address)+mailboxlen+1);
	if(full_address == NULL) {
		fprintf(stderr, "Error while creating IMAP address from domain.\n");
		free(mailbox_encoded);
		curl_easy_cleanup(curl);
		return -1;
	}
	//Building full address
	strcpy(full_address, address);
	free(address);
	strcat(full_address, mailbox_encoded);
	free(mailbox_encoded);

	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); //Toggle full logging
	curl_easy_setopt(curl, CURLOPT_PROTOCOLS, CURLPROTO_IMAPS);
	curl_easy_setopt(curl, CURLOPT_USERNAME, username);
	curl_easy_setopt(curl, CURLOPT_PASSWORD, password);

	curl_easy_setopt(curl, CURLOPT_URL,full_address);

	//enable_tls(curl);

	curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

	uid = ssl_search_by_id(curl, message_id);
	if(uid == 0) {
		fprintf(stderr, "No mail found with message ID %s.\n", message_id);
		curl_easy_cleanup(curl);
		return -1;
	}
	if(uid == -1) {
		fprintf(stderr, "An error occured when searching for mail uid.\n");
		curl_easy_cleanup(curl);
		return -1;
	}

	requestlen = strlen(request);
	sprintf(uidStr, "%d", uid); //Convert uid to string in order to get the amount of digits
	uidstrlen = strlen(uidStr);
	full_request = malloc(uidstrlen+requestlen+1);
	if(full_request == NULL) {
		fprintf(stderr, "Error while creating IMAP request.\n");
		curl_easy_cleanup(curl);
		return -1;
	}

	//Replacing format code with value
	sprintf(full_request, request, uid);

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_null); //Don't print the result to stdout
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST,full_request);

	res = curl_easy_perform(curl);

	/* Check for errors */
	if(res != CURLE_OK)
		fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));
	else {
		fputs("Request OK.\n", stdout);
	}

	/* Always cleanup */
	curl_easy_cleanup(curl);
	free(full_address);
	free(full_request);

	return (int)res;
}

/**
 * Performs a STORE (IMAP) operation to flag an email as seen (1) or unseen (0).
 */
int ssl_see_mail(char * username, char * password, char * domain, char * mailbox, char *message_id, char ssl, char seen) {
	return ssl_mail_request(username,password,domain,mailbox,message_id, ssl ,seen ? "STORE %d +Flags \\Seen" : "STORE %d -Flags \\Seen");
}

/**
 * Performs a STORE (IMAP) operation to flag an email as deleted.
 */
int ssl_delete_mail(char * username, char * password, char * domain, char * mailbox, char *message_id, char ssl) {
	return ssl_mail_request(username,password,domain,mailbox,message_id, ssl ,"STORE %d +Flags \\Deleted");
}

/**
 * Creates an Email, sets every pointer to NULL and returns the result.
 */
Email *init_email() {
	Email *mail = malloc(sizeof(Email));
	if(mail == NULL) return NULL;
	mail->date 		 = NULL;
	mail->from 		 = NULL;
	mail->message 	 = NULL;
	mail->subject 	 = NULL;
	mail->message_id = NULL;
	mail->to 		 = NULL;
	mail->in_reply_to= NULL;
	mail->references = NULL;
	mail->raw		 = NULL;
	mail->flags		 = NULL;
	mail->mailbox	 = NULL;
	return mail;
}

/**
 * Gets a substring of a header line using regex
 * Example: "From: email@address.io" returns "email@address.io"
 */
static char * parse_header_line(StringArray * content, char * regexp) {
	regex_t regex;
	regmatch_t pmatch[2];
	int len;
	char * dest = NULL;
	char * line;

	for(size_t i = 0 ; i < content->size-1 ; i++) {
		line = content->array[i];

		if(exec_regex(&regex, regexp, line, 2, &pmatch)) { //Use a regex to extract value
			len = pmatch[1].rm_eo - pmatch[1].rm_so;
			dest = malloc(len + 1);
			if(dest == NULL) {
				regfree(&regex);
				return NULL;
			}
			strncpy(dest, line+pmatch[1].rm_so, len);
			dest[len] = '\0';
			break;
		}
	}

	regfree(&regex);
	return dest;
}

/**
 * Parses a complete email payload (header + body) and returns the result into an Email struct. Returns NULL if an error occurred
 */
Email *parse_email(char * payload) {
	Email *mail = init_email();
	StringArray content;
	int len;
	char 	*date = NULL, *from = NULL,
			*to = NULL, *message = NULL,
			*subject = NULL, *message_id = NULL,
			*in_reply_to = NULL, *references = NULL,
			*raw = NULL;

	if(mail == NULL) {
		fputs("Could not allocate for new Email.\n", stderr);
		return NULL;
	}

	content = split_mail(payload);

	if(content.size <= 0) {
		fputs("Could not split mail payload.\n", stderr);
		return NULL;
	}

	//Parse date
	date = parse_header_line(&content, REGEX_DATE);
	if(date == NULL) {
		free_email(mail);
		free(mail);
		free_string_array(content);
		fputs("Couldn't extract date.\n", stderr);
		return NULL;
	}
	mail->date = date;

	//Parse TO
	to = parse_header_line(&content, REGEX_TO);
	if(to == NULL) {
		free_email(mail);
		free(mail);
		free_string_array(content);
		fputs("Couldn't extract TO.\n", stderr);
		return NULL;
	}
	mail->to = to;

	//Parse sender
	from = parse_header_line(&content, REGEX_FROM);
	if(from == NULL) {
		free_email(mail);
		free(mail);
		free_string_array(content);
		fputs("Couldn't extract FROM.\n", stderr);
		return NULL;
	}
	mail->from = from;

	//Parse subject
	subject = parse_header_line(&content, REGEX_SUBJECT);
	if(subject == NULL) {
		free_email(mail);
		free(mail);
		free_string_array(content);
		fputs("Couldn't extract subject.\n", stderr);
		return NULL;
	}
	mail->subject = subject;

	//Parse message id
	message_id = parse_header_line(&content, REGEX_MESSAGE_ID);
	if(message_id == NULL) {
		free_email(mail);
		free(mail);
		free_string_array(content);
		fputs("Couldn't extract message ID.\n", stderr);
		return NULL;
	}
	mail->message_id = message_id;

	//Parse in-reply-to
	in_reply_to = parse_header_line(&content, REGEX_IN_REPLY_TO);
	mail->in_reply_to = in_reply_to;
	//Don't check for NULL because this header is not present in every mail
	//Same for references

	//Parse references
	references = parse_header_line(&content, REGEX_REFERENCES);
	mail->references = references;

	//Simple copy of the message body
	len = content.array[content.size-1][0] == '\0' ? 0 : strlen(content.array[content.size-1]); //Check if message is empty
	message = malloc(len + 1);
	if(message == NULL) {
		free_email(mail);
		free(mail);
		free_string_array(content);
		fputs("Error while allocating for message.\n", stderr);
		return NULL;
	}

	if(len > 0)
		strcpy(message,content.array[content.size-1]);
	else
		message[0] = '\0';
	mail->message = message;

	//Copy the raw message
	raw = malloc(strlen(payload)+1);
	if(raw == NULL) {
		free_email(mail);
		free(mail);
		free_string_array(content);
		fputs("Error while allocating for raw message.\n", stderr);
		return NULL;
	}
	strcpy(raw,payload);
	mail->raw = raw;

	free_string_array(content);
	return mail;
}

/**
 * Safe free of an Email struct, ignoring NULL pointers
 */
void free_email(Email *email) {
	if(email == NULL) return;
	if(email->date != NULL)
		free(email->date);
	if(email->from != NULL)
		free(email->from);
	if(email->message != NULL)
		free(email->message);
	if(email->subject != NULL)
		free(email->subject);
	if(email->to != NULL)
		free(email->to);
	if(email->message_id != NULL)
		free(email->message_id);
	if(email->in_reply_to != NULL)
		free(email->in_reply_to);
	if(email->references != NULL)
		free(email->references);
	if(email->flags != NULL) {
		free_string_array(*email->flags);
		free(email->flags);
	}
	if(email->raw != NULL)
		free(email->raw);
	if(email->mailbox != NULL)
		free(email->mailbox);
}

/**
 * Moves an email from one folder to another performing a COPY (IMAP) operation then flags the mail as deleted in the source folder
 */
int ssl_move_mail(char * username, char * password, char * domain, char * mailbox_src, char * mailbox_dst, char *message_id, char ssl) {

	const char* request = "STORE %d +Flags \\Deleted";

	CURL *curl;
	CURLcode res = CURLE_OK;
	int mailboxlen_src = 0, mailboxlen_dst = 0, uidstrlen = 0, requestlen = 0;
	char * address;
	char * full_address;
	char * full_request;
	char * full_request2;
	char * mailbox_encoded_src;
	char * mailbox_encoded_dst;
	char uidStr[12];
	int uid;

	address = generate_address(domain, ssl ? "imaps": "imap");
	if(address == NULL) {
		fprintf(stderr, "Error while creating IMAP address from domain.\n");
		return -1;
	}
	if(mailbox_src == NULL) {
		fprintf(stderr, "mailbox is not nullable.\n");
		return -1;
	}

	curl = curl_easy_init();
	if(!curl) {
		fprintf(stderr, "Error on creating curl.\n");
		return -1;
	}

	mailbox_encoded_src = url_encode(curl, mailbox_src);
	if(mailbox_encoded_src == NULL) {
		fprintf(stderr, "Error on URL encoding.\n");
		return -1;
	}
	mailboxlen_src = strlen(mailbox_encoded_src);

	full_address = malloc(strlen(address)+mailboxlen_src+1);
	if(full_address == NULL) {
		fprintf(stderr, "Error while creating IMAP address from domain.\n");
		free(mailbox_encoded_src);
		return -1;
	}

	//Building full address
	strcpy(full_address, address);
	free(address);
	strcat(full_address, mailbox_encoded_src);

	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); //Toggle full logging
	curl_easy_setopt(curl, CURLOPT_PROTOCOLS, CURLPROTO_IMAPS);
	curl_easy_setopt(curl, CURLOPT_USERNAME, username);
	curl_easy_setopt(curl, CURLOPT_PASSWORD, password);

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_null); //Don't print the result to stdout

	curl_easy_setopt(curl, CURLOPT_URL,full_address);

	//enable_tls(curl);

	curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

	uid = ssl_search_by_id(curl, message_id);
	if(uid == 0) {
		fprintf(stderr, "No mail found with message ID %s.\n", message_id);
		free(full_address);
		curl_easy_cleanup(curl);
		return -1;
	}
	if(uid == -1) {
		fprintf(stderr, "An error occured when searching for mail uid.\n");
		free(full_address);
		curl_easy_cleanup(curl);
		return -1;
	}

	mailbox_encoded_dst = url_encode(curl, mailbox_dst);
	if(mailbox_encoded_dst == NULL) {
		fprintf(stderr, "Error on URL encoding.\n");
		free(full_address);
		curl_easy_cleanup(curl);
		return -1;
	}
	mailboxlen_dst = strlen(mailbox_encoded_dst);

	sprintf(uidStr, "%d", uid); //Convert uid to string in order to get the amount of digits and to insert into the request string
	uidstrlen = strlen(uidStr);


	full_request = malloc(6+uidstrlen+mailboxlen_dst+1);
	if(full_request == NULL) {
		fprintf(stderr, "Error while creating IMAP request.\n");
		free(full_address);
		free(full_request);
		curl_easy_cleanup(curl);
		return -1;
	}

	//Building request
	//COPY uid mailbox_dst
	strcpy(full_request, "COPY ");
	strcat(full_request, uidStr);
	strcat(full_request, " ");
	strcat(full_request, mailbox_encoded_dst);

	free(mailbox_encoded_src);
	free(mailbox_encoded_dst);

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_null); //Don't print the result to stdout
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST,full_request);

	res = curl_easy_perform(curl);

	/* Check for errors */
	if(res != CURLE_OK)
		fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));
	else {

		/* Check for errors */
		if(res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
					curl_easy_strerror(res));
		} else {
			fputs("Mail moved OK.\n", stdout);

			//Flag the original mail as deleted to avoid duplicates
			requestlen = strlen(request);
			sprintf(uidStr, "%d", uid); //Convert uid to string in order to get the amount of digits
			uidstrlen = strlen(uidStr);

			full_request2 = malloc(uidstrlen+requestlen+1);
			if(full_request2 == NULL) {
				fprintf(stderr, "Error while creating IMAP request.\n");
				curl_easy_cleanup(curl);
				free(full_request);
				free(full_address);
				return -1;
			}

			//Replacing format code with value
			sprintf(full_request2, request, uid);

			curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST,full_request2);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_null);

			res = curl_easy_perform(curl);

			/* Check for errors */
			if(res != CURLE_OK)
				fprintf(stderr, "curl_easy_perform() failed: %s\n",
						curl_easy_strerror(res));
			else {
				fputs("Original mail flagged as deleted OK.\n", stdout);

				curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "EXPUNGE"); //Tells the server to delete of mails flagged as deleted
				res = curl_easy_perform(curl);
			}

			free(full_request2);
		}

	}

	/* Always cleanup */
	curl_easy_cleanup(curl);
	free(full_address);
	free(full_request);

	return (int)res;
}

/**
 * Searches an email by Message-ID and returns the UID if found
 * Creates an new CURL connection
 * Returns 0 if not found, -1 if an error occurred
 */
int ssl_search_by_id_with_new_connection(char * username, char * password, char * domain, char * mailbox, char *message_id, char ssl) {
	CURL *curl;
	int mailboxlen = 0;
	char * address;
	char * full_address;
	char * mailbox_encoded;
	int result = -1;

	address = generate_address(domain, ssl ? "imaps" : "imap");
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
	full_address = malloc(strlen(address)+mailboxlen+1);
	if(full_address == NULL) {
		fprintf(stderr, "Error while creating IMAP address from domain.\n");
		free(mailbox_encoded);
		curl_easy_cleanup(curl);
		return -1;
	}
	//Building full address
	strcpy(full_address, address);
	free(address);
	strcat(full_address, mailbox_encoded);
	free(mailbox_encoded);

	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); //Toggle full logging
	curl_easy_setopt(curl, CURLOPT_PROTOCOLS, CURLPROTO_IMAPS);
	curl_easy_setopt(curl, CURLOPT_USERNAME, username);
	curl_easy_setopt(curl, CURLOPT_PASSWORD, password);

	enable_tls(curl);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
	curl_easy_setopt(curl, CURLOPT_URL,full_address);

	result = ssl_search_by_id(curl, message_id);

	free(full_address);
	return result;
}

/**
 * Searches an email by Message-ID and returns the UID if found
 * Uses an already existing CURL connection
 * Returns 0 if not found, -1 if an error occurred
 */
int ssl_search_by_id(CURL *curl, char *message_id) {
	CURLcode res = CURLE_OK;
	struct MemoryStruct chunk;
	char *full_request;
	int message_id_len = 0;
	int uid = -1;

	message_id_len = strlen(message_id);

	full_request = malloc(25+message_id_len+1); //25 for "SEARCH HEADER Message-ID "
	if(full_request == NULL) {
		fprintf(stderr, "Error while creating IMAP SEARCH request.\n");
		return -1;
	}

	strcpy(full_request, "SEARCH HEADER Message-ID ");
	strcat(full_request, message_id);

	chunk.memory = malloc(1); //Initial allocation. Will be reallocated in write_memory_callback() to fit the correct size.
	chunk.size = 0;

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_memory_callback);

	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST,  full_request);

	/* Perform the custom request */
	res = curl_easy_perform(curl);

	if(res != CURLE_OK)
		fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));

	else {
		if(strlen(chunk.memory) > 9) { //Result is not "* SEARCH" meaning that a mail with the given message_id was found
			uid = strtol(chunk.memory+9, NULL,10); //+9 to ignore "* SEARCH "
		}
	}

	free(full_request);
	free(chunk.memory);
	return uid;
}

void free_parsed_search(struct ParsedSearch *search) {
	if(search == NULL) return;
	if(search->uids != NULL)
		free(search->uids);
	free(search);
}

/**
 * Loads the email necessary headers into the LinkedList loaded_mails. Return 1 if success, 0 otherwise.
 */
int ssl_load_mail_headers(char * username, char * password, char * domain, char * mailbox, char ssl, struct ParsedSearch *search) {
	Email *mail = NULL;
	CURL *curl;
	CURLcode res = CURLE_OK;
	struct MemoryStruct headers;
	struct MemoryStruct chunk;
	int mailboxlen = 0, uidstrlen = 0;
	char * address;
	char * mailbox_encoded;
	char * full_address;
	char * full_request;
	char uidStr[12];

	address = generate_address(domain, ssl ? "imaps" : "imap");
	if(address == NULL) {
		fprintf(stderr, "Error while creating IMAP address from domain.\n");
		return 0;
	}
	if(mailbox == NULL) {
		fprintf(stderr, "mailbox is not nullable.\n");
		return 0;
	}

	curl = curl_easy_init();
	if(!curl) {
		fprintf(stderr, "Error on creating curl.\n");
		return 0;
	}
	mailbox_encoded = url_encode(curl, mailbox);
	if(mailbox_encoded == NULL) {
		fprintf(stderr, "Error on URL encoding.\n");
		curl_easy_cleanup(curl);
		return 0;
	}
	mailboxlen = strlen(mailbox_encoded);

	full_address = malloc(strlen(address)+mailboxlen+1);
	if(full_address == NULL) {
		fprintf(stderr, "Error while creating IMAP address from domain.\n");
		curl_easy_cleanup(curl);
		return 0;
	}

	//Building full address
	//imaps://domain/mailbox/
	strcpy(full_address, address);
	free(address);
	strcat(full_address, mailbox_encoded);
	free(mailbox_encoded);

	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); //Toggle full logging
	curl_easy_setopt(curl, CURLOPT_PROTOCOLS, CURLPROTO_IMAPS);
	curl_easy_setopt(curl, CURLOPT_USERNAME, username);
	curl_easy_setopt(curl, CURLOPT_PASSWORD, password);

	curl_easy_setopt(curl, CURLOPT_URL,full_address);

	enable_tls(curl);

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_memory_callback);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, write_memory_callback);

	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, (void *)&headers);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

	for(int i = search->size-1 ; i >= 0; i--) {

		sprintf(uidStr, "%d", search->uids[i]);
		uidstrlen = strlen(uidStr);
		//Request : FETCH uid (FLAGS BODY.PEEK[HEADER.FIELDS (SUBJECT DATE FROM TO MESSAGE-ID)])
		full_request = malloc(6+67+uidstrlen+1); //6 for "FETCH ", +67 for " (FLAGS BODY.PEEK[HEADER.FIELDS (SUBJECT DATE FROM TO MESSAGE-ID)])"
		if(full_request == NULL) {
			fprintf(stderr, "Error while creating IMAP request from domain.\n");
			curl_easy_cleanup(curl);
			return 0;
		}
		//Building request
		strcpy(full_request,"FETCH ");
		strcat(full_request, uidStr);
		strcat(full_request, " (FLAGS BODY.PEEK[HEADER.FIELDS (SUBJECT DATE FROM TO MESSAGE-ID)])");


		headers.memory = malloc(1); //Initial allocation. Will be reallocated in write_memory_callback() to fit the correct size.
		headers.size = 0;
		chunk.memory = malloc(1);
		chunk.size = 0;

		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST,  full_request);
		res = curl_easy_perform(curl);

		/* Check for errors */
		if(res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
					curl_easy_strerror(res));
		else {
			mail = parse_email_headers(headers.memory, chunk.memory, mailbox);
			if(mail == NULL) {
				fputs("Error, invalid mail headers payload.\n", stderr);
			} else {
				linkedlist_add(loaded_mails, mail);
			}

			free(chunk.memory);
			free(headers.memory);
		}

		/* Always cleanup */

		free(full_request);
	}

	curl_easy_cleanup(curl);
	free(full_address);
	return 1;
}

Email *parse_email_headers(char *payload, char *chunk, char *mailbox) {
	Email *mail = init_email();
	StringArray content;

	if(mail == NULL) {
		fputs("Could not allocate for new Email.\n", stderr);
		return NULL;
	}

	content = split_mail(payload);

	if(content.size <= 0) {
		fputs("Could not split mail headers payload.\n", stderr);
		return NULL;
	} else if(content.size < 6) {
		fputs("Invalid mail headers payload.\n", stderr);
		return NULL;
	}

	//Parse flags
	mail->flags = parse_flags(chunk, REGEX_FLAGS_HEADERS);
	if(mail->flags == NULL) {
		fputs("Couldn't extract flags.\n", stderr);
		free_email(mail);
		free(mail);
		return NULL;
	}

	//Parse FROM
	mail->from = parse_header_line(&content, REGEX_FROM);
	if(mail->from == NULL) {
		fputs("Couldn't extract FROM.\n", stderr);
		free_email(mail);
		free(mail);
		return NULL;
	}

	//Parse TO
	mail->to = parse_header_line(&content, REGEX_TO);
	if(mail->to == NULL) {
		fputs("Couldn't extract TO.\n", stderr);
		free_email(mail);
		free(mail);
		return NULL;
	}

	//Parse SUBJECT
	mail->subject = parse_header_line(&content, REGEX_SUBJECT);
	if(mail->subject == NULL) {
		fputs("Couldn't extract SUBJECT.\n", stderr);
		free_email(mail);
		free(mail);
		return NULL;
	}

	//Parse DATE
	mail->date = parse_header_line(&content, REGEX_DATE);
	if(mail->date == NULL) {
		fputs("Couldn't extract DATE.\n", stderr);
		free_email(mail);
		free(mail);
		return NULL;
	}

	//Parse Message-ID
	mail->message_id = parse_header_line(&content, REGEX_MESSAGE_ID);
	if(mail->message_id == NULL) {
		fputs("Couldn't extract Message-ID.\n", stderr);
		free_email(mail);
		free(mail);
		return NULL;
	}

	mail->mailbox = malloc(strlen(mailbox)+1);
	if(mail->mailbox == NULL) {
		fputs("Couldn't allocate for mailbox copy.\n", stderr);
		free_email(mail);
		free(mail);
		return NULL;
	} else {
		strcpy(mail->mailbox, mailbox);
	}

	free_string_array(content);
	return mail;
}
