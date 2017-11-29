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

	Email mail = parse_email("Date: Sun, 26 Nov 2017 16:22:33 +0100\r\nTo: <jeremy.la@outlook.fr>\r\nFrom: <jumailimap@gmail.com>(Ju Mail)\r\nMessage-ID: <d5252969-6b02-469a-a5d1-57b1305cc768@jumail.fr>\r\nSubject: Bonjour ça va?\r\n\r\nSalut je voulais prendre de tes nouvelles.\r\nÇa fait longtemps.\r\nBye.");
	/*fputs(mail.date, stdout);
	fputs("\n", stdout);
	fputs(mail.from, stdout);
	fputs("\n", stdout);
	fputs(mail.from_name, stdout);
	fputs("\n", stdout);
	fputs(mail.to, stdout);
	fputs("\n", stdout);
	fputs(mail.messageID, stdout);
	fputs("\n", stdout);
	fputs(mail.message, stdout);
	fputs("\n", stdout);*/
	free_email(mail);

	return 0;
	//return main_window_start(argc, argv);
}


