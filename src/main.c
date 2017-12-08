/**
 * 	main.c
 *
 *  Author : Jérémy LAMBERT
 *  Date : 19/11/2017
 *  Description : The main file
 */
#include "MainWindow.h"
#include "mailing.h"

#include <stdlib.h>
#include <time.h>

int main(int argc, char** argv) {
	//ssl_fetch("jumailimap@gmail.com", "azerty12", "imap.gmail.com", "INBOX");
	ssl_list("jumailimap@gmail.com", "azerty12", "imap.gmail.com");

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
		char** header = get_header("jumailimap@gmail.com", "jeremy.la@outlook.fr", "Ju Mail", "Test 07", id);
		if(header == NULL) {
			fprintf(stderr, "An error occured while creating the email header.\n");
			//Handle error
			//e.g. free id and exit
		}
		free(id);
		char ** mail = get_mail(header,"Contenu du message");
		if(mail == NULL) {
			fprintf(stderr, "An error occured while creating the email payload.\n");
			//Handle error
			//e.g. free header and exit
		}

		send_mail_ssl("jumailimap@gmail.com", "azerty12", "jeremy.la@outlook.fr", "smtp.gmail.com", (const char **)mail);
		free_header(header);
		free_mail(mail);*/


	return 0;
	//return main_window_start(argc, argv);
}


