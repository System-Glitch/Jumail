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
	ssl_fetch("jumailimap@gmail.com", "azerty12", "imap.gmail.com", "INBOX");
	return 0;
	//return main_window_start(argc, argv);
}


