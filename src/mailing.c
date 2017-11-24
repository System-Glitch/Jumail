/**
 * 	mailing.c
 *
 *  Author : Jérémy LAMBERT
 *  Date : 23/11/2017
 *  Description : Mailing utilities
 */
#include <stdlib.h>
#include <time.h>
#include "mailing.h"

static const char *payload_text[] = {
		"Date: Mon, 29 Nov 2010 21:54:29 +1100\r\n",
		"To: " TO "\r\n",
		"From: " FROM "(Ton père)\r\n",
		"Message-ID: "
		"<dcd7cb36-11db-487a-9f3b-e652a9458efd@jumail.fr>\r\n",
		"Subject: IMAP example message\r\n",
		"\r\n", /* empty line to divide headers from body, see RFC5322 */
		"The body of the message starts here.\r\n",
		"\r\n",
		"It could be a lot of lines, could be MIME encoded, whatever.\r\n",
		"Check RFC5322.\r\n",
		NULL
};

struct upload_status {
	int lines_read;
};

static char * get_date() {
	char * date = malloc(100);
	time_t now = time(NULL);
	struct tm *t = localtime(&now);

	strftime(date, sizeof(date)-1, "Date: %a, %d %h %Y %H:%M:%S %z\r\n", t);
	return date;
}

static void enable_ssl(CURL *curl) {
#ifdef SKIP_PEER_VERIFICATION
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
#endif

#ifdef SKIP_HOSTNAME_VERIFICATION
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
#endif
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
}

static size_t write_memory_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
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

static size_t payload_source(void *ptr, size_t size, size_t nmemb, void *userp)
{
	struct upload_status *upload_ctx = (struct upload_status *)userp;
	const char *data;

	if((size == 0) || (nmemb == 0) || ((size*nmemb) < 1)) {
		return 0;
	}

	data = payload_text[upload_ctx->lines_read];

	if(data) {
		size_t len = strlen(data);
		memcpy(ptr, data, len);
		upload_ctx->lines_read++;

		return len;
	}

	return 0;
}

//Gets GMAIL folders
int ssl_fetch() {
	CURL *curl;
	CURLcode res = CURLE_OK;
	struct MemoryStruct chunk;
	struct MemoryStruct headers;

	chunk.memory = malloc(1);  /* will be grown as needed by the realloc above */
	chunk.size = 0;
	headers.memory = malloc(1);  /* will be grown as needed by the realloc above */
	headers.size = 0;

	curl = curl_easy_init();
	if(curl) {


		curl_easy_setopt(curl, CURLOPT_PROTOCOLS, CURLPROTO_IMAPS);

		/* Set username and password */
		curl_easy_setopt(curl, CURLOPT_USERNAME, "jumailimap@gmail.com");
		curl_easy_setopt(curl, CURLOPT_PASSWORD, "azerty12");

																			//INBOX?ALL returns * SEARCH 1 2 3 4
		curl_easy_setopt(curl, CURLOPT_URL,"imaps://imap.gmail.com:993/"); // INBOX/;UID=x

		enable_ssl(curl);

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_memory_callback);

		/* we pass our 'chunk' struct to the callback function */
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
		curl_easy_setopt(curl, CURLOPT_HEADERDATA, (void *)&headers);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

		/* Perform the fetch */
		res = curl_easy_perform(curl);

		/* Check for errors */
		if(res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
					curl_easy_strerror(res));
		else {
			fputs(headers.memory, stdout);
			fputs(chunk.memory, stdout);
		}

		free(headers.memory);
		free(chunk.memory);
		/* Always cleanup */
		curl_easy_cleanup(curl);
	}

	return (int)res;
}

int ssl_append() {
	CURL *curl;
	CURLcode res = CURLE_OK;
	const char **p;
	long infilesize;
	struct upload_status upload_ctx;

	upload_ctx.lines_read = 0;

	curl = curl_easy_init();
	if(curl) {
		/* Set username and password */
		curl_easy_setopt(curl, CURLOPT_USERNAME, "jumailimap@gmail.com");
		curl_easy_setopt(curl, CURLOPT_PASSWORD, "azerty12");

		curl_easy_setopt(curl, CURLOPT_URL,"imaps://imap.gmail.com:993/INBOX");

		enable_ssl(curl);

		curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
		curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
		curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

		infilesize = 0;
		for(p = payload_text; *p; ++p) {
			infilesize += (long)strlen(*p);
		}
		curl_easy_setopt(curl, CURLOPT_INFILESIZE, infilesize);

		res = curl_easy_perform(curl);

		/* Check for errors */
		if(res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

		/* Always cleanup */
		curl_easy_cleanup(curl);
	}

	return (int)res;
}
