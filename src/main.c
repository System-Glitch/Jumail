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
	ssl_search_all("jumailimap@gmail.com", "azerty12", "imap.gmail.com", "[Gmail]/Messages envoyés");
	/*StringArray *list = ssl_list("jumailimap@gmail.com", "azerty12", "imap.gmail.com");
	for(int i = 0 ; i < list->size ; i++) {
		fputs(list->array[i], stdout);
		fputs("\n", stdout);
	}
	free_string_array(*list);*/

	//ssl_move_mail("jumailimap@gmail.com", "azerty12", "imap.gmail.com", "INBOX", "Test", "<8tA94c5sdm2AvdsW99Fv3A@notifications.google.com>");
	//ssl_remove_folder("jumailimap@gmail.com", "azerty12", "imap.gmail.com", "yolé y ouh");

	/*Email mail = parse_email("Date: Sun, 26 Nov 2017 16:22:33 +0100\r\nTo: <jeremy.la@outlook.fr>\r\nFrom: <jumailimap@gmail.com>(Ju Mail)\r\nMessage-ID: <d5252969-6b02-469a-a5d1-57b1305cc768@jumail.fr>\r\nSubject: Bonjour ça va?\r\n\r\nSalut je voulais prendre de tes nouvelles.\r\nÇa fait longtemps.\r\nBye.",0);
	fputs(mail.date, stdout);
	fputs("\n", stdout);
	fputs(mail.from, stdout);
	fputs("\n", stdout);
	fputs(mail.to, stdout);
	fputs("\n", stdout);
	fputs(mail.message, stdout);
	printf("\n%d\n", mail.uid);
	free_email(mail);*/

	//SENDING A MAIL

	/*char * id = generate_id();
	if(id == NULL) {
		fprintf(stderr, "An error occured while getting a new GUID. Check your internet connection.\n");
		exit(1);
	}
	char** header = get_header("jumailimap@gmail.com", "jumailimap@gmail.com", "Ju Mail", "Test reply multiple", "<2b5a344b-5826-4492-84c4-4b6d5bb5fc9c@jumail.fr>", NULL, id);
	if(header == NULL) {
		fprintf(stderr, "An error occured while creating the email header.\n");
		//Handle error
		//e.g. free id and exit
	}
	free(id);
	char ** mail = get_mail(header,"Contenu du message test reply");
	if(mail == NULL) {
		fprintf(stderr, "An error occured while creating the email payload.\n");
		//Handle error
		//e.g. free header and exit
	}

	send_mail_ssl("jumailimap@gmail.com", "azerty12", "jumailimap@gmail.com", "smtp.gmail.com", (const char **)mail);
	free_header(header);
	free_mail(mail);*/

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


