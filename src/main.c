/**
 * 	main.c
 *
 *  Author : Jérémy LAMBERT
 *  Date : 19/11/2017
 *  Description : The main file
 */
#include "MainWindow.h"
#include "mailing.h"
#include "folders.h"
#include "libs/muttx/utf7.h"

int main(int argc, char** argv) {

	/*Email *mail = ssl_get_mail("jumailimap@gmail.com", "azerty12", "imap.gmail.com", "INBOX", 5);
	printf("Date: %s\n", mail->date);
	printf("Message-ID: %s\n", mail->message_id);
	printf("From: %s\n", mail->from);
	printf("To: %s\n", mail->to);
	printf("Subject: %s\n", mail->subject);

	if(mail->in_reply_to != NULL)
		printf("In-Reply-To: %s\n", mail->in_reply_to); //Message-ID of the parent mail

	if(mail->references != NULL)
			printf("References: %s\n", mail->references); //Content of the References header. See RFC2822

	printf("Flags: ");
	for(size_t i = 0 ; i < mail->flags->size ; i++) { //Flags such as seen/unseen, phishing, draft, etc
		printf(" %s", mail->flags->array[i]);
	}
	printf("\n");

	printf("Message: \n%s\n", mail->message);

	free_email(mail); //Always free*/

	/*struct ParsedSearch *search = ssl_search_all("jumailimap@gmail.com", "azerty12", "imap.gmail.com", "INBOX");
	for(size_t i = 0 ; i < search->size ; i++) {
		printf("%d\n",search->uids[i]);
	}
	free_parsed_search(search); //Always free*/
	/*StringArray *list = ssl_list("jumailimap@gmail.com", "azerty12", "imap.gmail.com");
	for(int i = 0 ; i < list->size ; i++) {
		fputs(list->array[i], stdout);
		fputs("\n", stdout);
	}
	free_string_array(*list);
	free(list);*/

	//ssl_move_mail("jumailimap@gmail.com", "azerty12", "imap.gmail.com", "INBOX", "Test", "<8tA94c5sdm2AvdsW99Fv3A@notifications.google.com>");
	//ssl_remove_folder("jumailimap@gmail.com", "azerty12", "imap.gmail.com", "yolé y ouh");

	//SENDING A MAIL

	char * id = generate_id(); //Must generate a unique Message-ID for our mail
	if(id == NULL) {
		fprintf(stderr, "An error occured while getting a new GUID. Check your internet connection.\n");
		exit(1);
	}
	char** header = get_header("jumailimap@gmail.com", "jumailimap@gmail.com", "Ju Mail", "Test send", NULL, NULL, id); //Generate the header
	if(header == NULL) {
		fprintf(stderr, "An error occured while creating the email header.\n");
		//Handle error
		//e.g. free id and exit
	}
	free(id); //We don't need "id" anymore as it is copied into the header
	char ** mail = get_mail(header,"Contenu du message"); //Generate the whole payload
	if(mail == NULL) {
		fprintf(stderr, "An error occured while creating the email payload.\n");
		//Handle error
		//e.g. free header and exit
	}

	send_mail_ssl("jumailimap@gmail.com", "azerty12", "jumailimap@gmail.com", "smtp.gmail.com", (const char **)mail);
	free_header(header); //The header is a two dimensional array. It must be freed using this function
	free_mail(mail); //Always free

	/*char *str = malloc(10);
	char *str_encoded;
	strcpy(str, "yolé y ouh");

	fputs(str, stdout);
	fputs("\n", stdout);

	utf8_to_utf7(str, strlen(str), &str_encoded, 0);

	fputs(str_encoded, stdout);
	fputs("\n", stdout);

	free(str_encoded);
	free(str);*/


	return 0;
	//return main_window_start(argc, argv);
}


