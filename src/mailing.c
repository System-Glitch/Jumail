/**
 * 	mailing.c
 *
 *  Author : Jérémy LAMBERT
 *  Date : 23/11/2017
 *  Description : Mailing utilities
 */
#include "mailing.h"

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

	strftime(date, 79, "Date: %a, %d %m %Y %H:%M:%S %z\r\n", t); //Linux : "Date: %a, %d %h %Y %H:%M:%S %z\r\n"
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
	idlen = strlen(id)+1;
	if(idlen <= 0) return NULL;

	result = malloc(idlen+26); //+13 for "Message-ID: <", +13 for "\r\n"
	if(result == NULL) return NULL;

	strcpy(result, "Message-ID: <");
	strcat(result, id);
	strcat(result, "@jumail.fr>\r\n");
	return result;
}

/**
 * Generates the full mail header. "name" is nullable. Returns NULL if an error occurred or some inputs are wrong.
 * Don't forget to free it.
 */
char ** get_header(char * from, char * to, char * name, char * subject, char* id) {
	char ** header;
	char * fromline = get_from(from, name);
	char * toline = get_to(to);
	char * dateline = get_date();
	char * idline = get_id(id);
	char * subjectline = get_subject(subject);
	char * separatorline;

	if(fromline == NULL || toline == NULL || dateline == NULL ||
			subjectline == NULL || idline == NULL) return NULL;

	separatorline = malloc(3);
	if(separatorline == NULL) return NULL;
	strcpy(separatorline, "\r\n");

	header = malloc(6*sizeof(char *));
	header[0] = dateline;
	header[1] = toline;
	header[2] = fromline;
	header[3] = idline;
	header[4] = subjectline;
	header[5] = separatorline;
	return header;
}

void free_header(char ** header) {
	for(int i = 0 ; i < 6 ; i++)
		free(header[i]);
	free(header);
}

/**
 * Generates the whole message payload. Returns NULL if an error occurred or if invalid parameters are given.
 * Don't forget to free it.
 */
char ** get_mail(char ** header, char * message) {
	char ** mail;

	if(header == NULL || message == NULL) return NULL;

	mail = malloc(8*sizeof(char *));
	if(mail == NULL) return NULL;

	for(int i = 0 ; i < 6 ; i++) { //Copy the header
		mail[i] = malloc(strlen(header[i])+1);
		if(mail[i] == NULL) return NULL;
		strcpy(mail[i], header[i]);
	}
	mail[6] = malloc(strlen(message)+1);
	if(mail[6] == NULL) return NULL;
	strcpy(mail[6], message); //Copy the message

	mail[7] = NULL; //End Delimiter

	return mail;
}

void free_mail(char ** mail) {
	for(int i = 0 ; i < 7 ; i++)
		free(mail[i]);
	free(mail);
}

//SENDING A MAIL
/*
 	char * id = generate_id();
	if(id == NULL) {
		fprintf(stderr, "An error occured while getting a new GUID. Check your internet connection.\n");
		exit(1);
	}
	char** header = get_header("jumailimap@gmail.com", "jeremy.la@outlook.fr", "Ju Mail", "Test5", id);
	if(header == NULL) {
		fprintf(stderr, "An error occured while creating the email header.\n");
		//Handle error
		//e.g. free id and exit
	}
	free(id);
	char ** mail = get_mail(header,"Yolo");
	if(mail == NULL) {
		fprintf(stderr, "An error occured while creating the email payload.\n");
		//Handle error
		//e.g. free header and exit
	}

	send_mail_ssl("jumailimap@gmail.com", "azerty12", "jeremy.la@outlook.fr", "smtp.gmail.com", (const char **)mail);
	free_header(header);
	free_mail(mail);
 */

/**
 * Sends a mail generated with get_mail using SMTP.
 */
int send_mail_ssl(char * username, char * password, char * to, char * domain, const char ** mail) {
	CURL *curl;
	CURLcode res = CURLE_OK;
	struct curl_slist *recipients = NULL;
	const char **p;
	long infilesize;
	struct upload_status upload_ctx;
	char * address;

	address = generate_address(domain, "smtps");
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

		enable_ssl(curl);

		curl_easy_setopt(curl, CURLOPT_MAIL_FROM, username);

		recipients = curl_slist_append(recipients, to);
		curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

		curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
		curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
		curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

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
 * Parses a String result of the FETCH operation (IMAP) into an integer array containing all the UIDs.
 */
static struct ParsedSearch parse_search(char * answer) {
	struct ParsedSearch search;
	char * sub;
	char * end;
	int * result = NULL;
	int size, len, len2, i, index;

	search.uids = NULL;
	search.size = 0;

	size = strcount(answer, ' ') - 1;
	len = strlen(answer);
	len2 = len - 9;
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
			int n = strtol(sub+index, &end, 10);
			result[i++] = n;
			while (*end == ' ') {
				end++;
			}
			if(*end == '\r' || *end == '\n') break;
			index = end - sub;
		}
		free(sub);
	}

	search.uids = result;
	search.size = size;

	return search;
}

int ssl_fetch(char * username, char * password, char * domain, char * mailbox) {
	CURL *curl;
	CURLcode res = CURLE_OK;
	struct MemoryStruct chunk;
	struct ParsedSearch search;
	int mailboxlen = 0;
	char * address;
	char * full_address;
	char * mailbox_encoded;

	address = generate_address(domain, "imaps");
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
		return -1;
	}
	mailboxlen = strlen(mailbox_encoded);
	full_address = malloc(strlen(address)+mailboxlen+4+1);
	if(full_address == NULL) {
		fprintf(stderr, "Error while creating IMAP address from domain.\n");
		return -1;
	}
	strcpy(full_address, address);
	free(address);
	strcat(full_address, mailbox_encoded);
	strcat(full_address, "?ALL");
	free(mailbox_encoded);

	chunk.memory = malloc(1);
	chunk.size = 0;

	if(curl) {
		curl_easy_setopt(curl, CURLOPT_PROTOCOLS, CURLPROTO_IMAPS);
		curl_easy_setopt(curl, CURLOPT_USERNAME, username);
		curl_easy_setopt(curl, CURLOPT_PASSWORD, password);
		curl_easy_setopt(curl, CURLOPT_URL,full_address);

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
			search = parse_search(chunk.memory);
			if(search.uids == NULL) {
				fprintf(stderr, "Search is NULL\n");
				return -1;
			}
			for(int i = 0 ; i < search.size ; i++) {
				ssl_get_mail(username,password,domain,mailbox,search.uids[i]); //TODO
			}
			free(search.uids);
		}

		free(chunk.memory);
		/* Always cleanup */
		curl_easy_cleanup(curl);
	}
	free(full_address);

	return (int)res;
}

int ssl_get_mail(char * username, char * password, char * domain, char * mailbox, int uid) {
	CURL *curl;
	CURLcode res = CURLE_OK;
	struct MemoryStruct chunk;
	int mailboxlen = 0, uidstrlen = 0;
	char * address;
	char * mailbox_encoded;
	char * full_address;
	char uidStr[12];

	address = generate_address(domain, "imaps");
	if(address == NULL) {
		fprintf(stderr, "Error while creating IMAP address from domain.\n");
		return -1;
	}
	if(mailbox == NULL) {
		fprintf(stderr, "mailbox is not nullable.\n");
		return -1;
	}

	sprintf(uidStr, "%d", uid);
	uidstrlen = strlen(uidStr);

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

	full_address = malloc(strlen(address)+mailboxlen+uidstrlen+6+1);
	if(full_address == NULL) {
		fprintf(stderr, "Error while creating IMAP address from domain.\n");
		curl_easy_cleanup(curl);
		return -1;
	}
	strcpy(full_address, address);
	free(address);
	strcat(full_address, mailbox_encoded);
	strcat(full_address, "/;UID=");
	strcat(full_address, uidStr);
	free(mailbox_encoded);

	chunk.memory = malloc(1);
	chunk.size = 0;

	if(curl) {
		curl_easy_setopt(curl, CURLOPT_PROTOCOLS, CURLPROTO_IMAPS);
		curl_easy_setopt(curl, CURLOPT_USERNAME, username);
		curl_easy_setopt(curl, CURLOPT_PASSWORD, password);

		curl_easy_setopt(curl, CURLOPT_URL,full_address);

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
			fputs("Chunk : ",stdout); //TODO
			fputs(chunk.memory, stdout);
			fflush(stdout);
			/*Email mail = parse_email(chunk.memory,uid);
			fputs(mail.date, stdout);
			fputs("\n", stdout);
			fputs(mail.from, stdout);
			fputs("\n", stdout);
			fputs(mail.to, stdout);
			fputs("\n", stdout);
			fputs(mail.subject, stdout);
			fputs("\n", stdout);
			fputs(mail.message, stdout);
			printf("\n%d\n", mail.uid);
			free_email(mail);*/
		}

		free(chunk.memory);
		/* Always cleanup */
		curl_easy_cleanup(curl);
	}
	free(full_address);

	return (int)res;
}

int ssl_mail_request(char * username, char * password, char * domain, char * mailbox, int uid, const char *request) {
	CURL *curl;
	CURLcode res = CURLE_OK;
	int mailboxlen = 0, uidstrlen = 0, requestlen = 0;
	char * address;
	char * full_address;
	char * full_request;
	char * mailbox_encoded;
	char uidStr[12];

	address = generate_address(domain, "imaps");
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

	sprintf(uidStr, "%d", uid);
	uidstrlen = strlen(uidStr);
	mailboxlen = strlen(mailbox_encoded);
	requestlen = strlen(request);
	full_address = malloc(strlen(address)+mailboxlen+1);
	if(full_address == NULL) {
		fprintf(stderr, "Error while creating IMAP address from domain.\n");
		curl_easy_cleanup(curl);
		return -1;
	}

	full_request = malloc(uidstrlen+requestlen+1);
	if(full_request == NULL) {
		fprintf(stderr, "Error while creating IMAP request.\n");
		curl_easy_cleanup(curl);
		return -1;
	}
	strcpy(full_address, address);
	free(address);
	strcat(full_address, mailbox_encoded);
	free(mailbox_encoded);

	sprintf(full_request, request, uid);

	if(curl) {
		curl_easy_setopt(curl, CURLOPT_PROTOCOLS, CURLPROTO_IMAPS);
		curl_easy_setopt(curl, CURLOPT_USERNAME, username);
		curl_easy_setopt(curl, CURLOPT_PASSWORD, password);

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_null); //Don't print the result to stdout

		curl_easy_setopt(curl, CURLOPT_URL,full_address);
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST,full_request);

		enable_ssl(curl);

		curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

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
	}
	free(full_address);
	free(full_request);

	return (int)res;
}

int ssl_see_mail(char * username, char * password, char * domain, char * mailbox, int uid, char seen) {
	return ssl_mail_request(username,password,domain,mailbox,uid, seen ? "STORE %d +Flags \\Seen" : "STORE %d -Flags \\Seen");
}

int ssl_delete_mail(char * username, char * password, char * domain, char * mailbox, int uid) {
	return ssl_mail_request(username,password,domain,mailbox,uid,"STORE %d +Flags \\Deleted");
}

static Email init_email() {
	Email mail;
	mail.date = NULL;
	mail.from = NULL;
	mail.message = NULL;
	mail.subject = NULL;
	mail.to = NULL;
	return mail;
}

static char * parse_header_line(StringArray * content, char * regexp) {
	regex_t regex;
	regmatch_t pmatch[2];
	int len;
	char * dest = NULL;
	char * line;

	for(int i = 0 ; i < content->size-1 ; i++) {
		line = content->array[i];

		if(exec_regex(&regex, regexp, line, 2, &pmatch)) {
			len = pmatch[1].rm_eo - pmatch[1].rm_so;
			dest = malloc(len + 1);
			if(dest == NULL) {
				regfree(&regex);
				return NULL;
			}
			strncpy(dest, line+pmatch[1].rm_so, len);
			dest[len] = '\0';
		}
	}

	regfree(&regex);
	return dest;
}

Email parse_email(char * payload, int uid) {
	Email mail = init_email();
	StringArray content;
	int len;
	char *date = NULL, *from = NULL, *to = NULL, *message = NULL, *subject = NULL;

	content = split_mail(payload);

	if(content.size < 0) {
		fputs("Could not split mail payload.\n", stderr);
		return mail;
	}

	date = parse_header_line(&content, REGEX_DATE);
	if(date == NULL) {
		free_string_array(content);
		fputs("Couldn't extract date.\n", stderr);
		return mail;
	}

	to = parse_header_line(&content, REGEX_TO);
	if(to == NULL) {
		free_string_array(content);
		fputs("Couldn't extract TO.\n", stderr);
		return mail;
	}

	from = parse_header_line(&content, REGEX_FROM);
	if(from == NULL) {
		free_string_array(content);
		fputs("Couldn't extract FROM.\n", stderr);
		return mail;
	}

	subject = parse_header_line(&content, REGEX_SUBJECT);
	if(subject == NULL) {
		free_string_array(content);
		fputs("Couldn't extract subject.\n", stderr);
		return mail;
	}

	//Simple copy of the message body
	len = content.array[content.size-1][0] == '\0' ? 0 : strlen(content.array[content.size-1]); //Check if message is empty
	message = malloc(len + 1);
	if(message == NULL) {
		free_string_array(content);
		fputs("Error while allocating for message.\n", stderr);
		return mail;
	}

	if(len > 0)
		strcpy(message,content.array[content.size-1]);
	else
		message[0] = '\0';

	//Fill the struct
	mail.date = date;
	mail.from = from;
	mail.message = message;
	mail.subject = subject;
	mail.to = to;
	mail.uid = uid;

	free_string_array(content);
	return mail;
}

void free_email(Email email) {
	if(email.date != NULL)
		free(email.date);
	if(email.from != NULL)
		free(email.from);
	if(email.message != NULL)
		free(email.message);
	if(email.subject != NULL)
		free(email.subject);
	if(email.to != NULL)
		free(email.to);
}

int ssl_move_mail(char * username, char * password, char * domain, char * mailbox_src, char * mailbox_dst, int uid) {

	CURL *curl;
	CURLcode res = CURLE_OK;
	int mailboxlen_src = 0, mailboxlen_dst = 0, uidstrlen = 0;
	char * address;
	char * full_address;
	char * full_request;
	char * mailbox_encoded_src;
	char * mailbox_encoded_dst;
	char uidStr[12];

	address = generate_address(domain, "imaps");
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
		curl_easy_cleanup(curl);
		return -1;
	}
	mailboxlen_src = strlen(mailbox_encoded_src);

	mailbox_encoded_dst = url_encode(curl, mailbox_dst);
	if(mailbox_encoded_dst == NULL) {
		fprintf(stderr, "Error on URL encoding.\n");
		curl_easy_cleanup(curl);
		return -1;
	}
	mailboxlen_dst = strlen(mailbox_encoded_dst);

	sprintf(uidStr, "%d", uid);
	uidstrlen = strlen(uidStr);

	full_address = malloc(strlen(address)+mailboxlen_src+1);
	if(full_address == NULL) {
		fprintf(stderr, "Error while creating IMAP address from domain.\n");
		return -1;
	}

	full_request = malloc(6+uidstrlen+mailboxlen_dst+1);
	if(full_request == NULL) {
		fprintf(stderr, "Error while creating IMAP request.\n");
		return -1;
	}
	strcpy(full_address, address);
	free(address);
	strcat(full_address, mailbox_encoded_src);

	strcpy(full_request, "COPY ");
	strcat(full_request, uidStr);
	strcat(full_request, " ");
	strcat(full_request, mailbox_encoded_dst);

	free(mailbox_encoded_src);
	free(mailbox_encoded_dst);

	if(curl) {
		curl_easy_setopt(curl, CURLOPT_PROTOCOLS, CURLPROTO_IMAPS);
		curl_easy_setopt(curl, CURLOPT_USERNAME, username);
		curl_easy_setopt(curl, CURLOPT_PASSWORD, password);

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_null); //Don't print the result to stdout

		curl_easy_setopt(curl, CURLOPT_URL,full_address);
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST,full_request);

		enable_ssl(curl);

		curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

		res = curl_easy_perform(curl);

		/* Check for errors */
		if(res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
					curl_easy_strerror(res));
		else {
			curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "EXPUNGE");

			/* Perform the second custom request */
			res = curl_easy_perform(curl);

			/* Check for errors */
			if(res != CURLE_OK)
				fprintf(stderr, "curl_easy_perform() failed: %s\n",
						curl_easy_strerror(res));

			fputs("Mail moved OK.\n", stdout);
		}

		/* Always cleanup */
		curl_easy_cleanup(curl);
	}
	free(full_address);
	free(full_request);

	if(res != CURLE_OK)
		return (int)res;

	return ssl_delete_mail(username, password, domain, mailbox_src, uid);
}
