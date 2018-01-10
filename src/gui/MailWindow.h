/**
 * 	MailWindow.h
 *
 *  Author : Jérémy LAMBERT
 *  Date : 02/01/2018
 *  Description : Handler for the mail window
 */
#ifndef SRC_GUI_MAILWINDOW_H_
#define SRC_GUI_MAILWINDOW_H_

#include "../mailing.h"
#include "callbacks.h"

/**
 * Opens a mail window showing the given mail
 */
int open_mail_window(Email *mail, char* mailbox, SGlobalData *data);

/**
 * Clears the mail window and close it
 */
void mail_window_clear(SGlobalData *data);

/**
 * Opens the mail composing window
 */
void open_compose_mail_window(SGlobalData *data);

int open_mail_window_from_file(Email *mail, SGlobalData *data);

#endif /* SRC_GUI_MAILWINDOW_H_ */
