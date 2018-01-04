/**
 * 	callbacks.c
 *
 *  Author : Jérémy LAMBERT
 *  Date : 02/01/2018
 *  Description : Holds all the callbacks for the GUI
 */
#include <stdio.h>
#include "callbacks.h"
#include "TreeBrowsing.h"
#include "MailWindow.h"

enum Action action = NONE;

/**
 * Displays a modal confirm dialog and calls callback_confirm_response
 */
void show_confirm_dialog(const char * message, SGlobalData *data, char *parent_window_name) {
	GtkWidget *dialog;
	GtkWidget *main_window;

	main_window = GTK_WIDGET(gtk_builder_get_object (data->builder, parent_window_name));

	dialog = gtk_message_dialog_new (GTK_WINDOW(main_window), GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING, GTK_BUTTONS_YES_NO, "%s", message);
	g_signal_connect (dialog, "response", G_CALLBACK (callback_confirm_response), data);
	gtk_dialog_run (GTK_DIALOG(dialog));
	gtk_widget_destroy (dialog);
}

/**
 * Displays a modal error dialog.
 */
void window_show_error(const char * message, SGlobalData *data, char *parent_window_name) {
	GtkWidget *dialog;
	GtkWidget *main_window;

	main_window = GTK_WIDGET(gtk_builder_get_object (data->builder, parent_window_name));

	dialog = gtk_message_dialog_new (GTK_WINDOW(main_window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "%s", message);
	gtk_dialog_run (GTK_DIALOG(dialog));
	gtk_widget_destroy (dialog);
}

/**
 * Displays a modal success dialog.
 */
void window_show_info(const char * message, SGlobalData *data, char *parent_window_name) {
	GtkWidget *dialog;
	GtkWidget *main_window;

	main_window = GTK_WIDGET(gtk_builder_get_object (data->builder, parent_window_name));

	dialog = gtk_message_dialog_new (GTK_WINDOW(main_window), GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE, "%s", message);
	gtk_dialog_run (GTK_DIALOG(dialog));
	gtk_widget_destroy (dialog);
}

void callback_quit(GtkMenuItem *menuitem, gpointer user_data) {
	SGlobalData *data = (SGlobalData*) user_data;

	fputs("Closing GUI...\n", stdout);
	gtk_main_quit();
	fputs("Cleaning current mail...", stdout);
	free_email(data->current_email);
	if(data->current_email != NULL)
		free(data->current_email);
}

void callback_confirm_response(GtkDialog *dialog, gint response_id, gpointer user_data) {
	SGlobalData *data = (SGlobalData*) user_data;
	GtkTreeStore *tree_store;
	GtkListStore *list_store;
	GtkWidget *tree_view;
	gchar *string;
	GtkTreeIter iter;
	Email *mail;

	if(response_id == -8) { //YES

		gtk_widget_hide(GTK_WIDGET(dialog));
		switch(action) {
		case DELETE_FOLDER:
			mail_window_clear(data);
			tree_view = GTK_WIDGET(gtk_builder_get_object (data->builder, "TreeViewBrowsing"));
			tree_browsing_get_selected_row(data, &string, &iter);
			if(ssl_remove_folder("jumailimap@gmail.com", "azerty12", "imap.gmail.com", string)) { //TODO profile
				window_show_error("Impossible de supprimer le dossier.\nVérifiez votre connexion internet et les paramètres de votre profil.", data, "MainWindow");
			} else {
				//Remove folder from GUI
				tree_store = GTK_TREE_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(tree_view)));
				gtk_tree_store_remove (tree_store, &iter);
				browsing_refresh_folder(NULL, data);
			}
			break;
		case CREATE_FOLDER:
		case MOVE_MAIL:
		case MOVE_MAIL_FROM_VIEW:
			window_show_error("Une erreur est survenue.\nAction invalide pour cette fonction.", data, "MainWindow");
			break;
		case DELETE_MAIL:
			mail_window_clear(data);
			tree_view = GTK_WIDGET(gtk_builder_get_object (data->builder, "TreeViewFolderList"));
			int i = list_folder_get_selected_row(data, &iter);
			if(i >= 0) {
				mail = linkedlist_get(loaded_mails, i);
				if(ssl_delete_mail("jumailimap@gmail.com", "azerty12", "imap.gmail.com", mail->mailbox ,mail->message_id) != 0) { //TODO profile
					window_show_error("Impossible de supprimer le message.\nVérifiez votre connexion internet et les paramètres de votre profil.", data, "MainWindow");
				} else {
					//Remove mail from GUI
					list_store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(tree_view)));
					gtk_list_store_remove (list_store, &iter);

					free_email(mail);
					linkedlist_remove_index(loaded_mails, i);
				}
			} else {
				window_show_error("Une erreur est survenue.\nAucun message sélectionné.", data, "MainWindow");
			}

			break;
		case DELETE_MAIL_FROM_VIEW:
			i = data->selected_mail_index;
			mail_window_clear(data);
			tree_view = GTK_WIDGET(gtk_builder_get_object (data->builder, "TreeViewFolderList"));
			if(i >= 0) {
				mail = linkedlist_get(loaded_mails, i);
				if(ssl_delete_mail("jumailimap@gmail.com", "azerty12", "imap.gmail.com", mail->mailbox ,mail->message_id) != 0) { //TODO profile
					window_show_error("Impossible de supprimer le message.\nVérifiez votre connexion internet et les paramètres de votre profil.", data, "MailWindow");
				} else {
					//Remove mail from GUI
					list_store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(tree_view)));

					gtk_tree_model_get_iter_first(GTK_TREE_MODEL(list_store), &iter);
					for(int j = 0 ; j < i ; j++)
						gtk_tree_model_iter_next (GTK_TREE_MODEL(list_store), &iter);

					gtk_list_store_remove (list_store, &iter);

					free_email(mail);
					linkedlist_remove_index(loaded_mails, i);
					window_show_info("Message supprimé.", data, "MainWindow");
				}
			} else {
				window_show_error("Une erreur est survenue.\nAucun message sélectionné.", data, "MailWindow");
			}
			break;
		case NONE:
			window_show_error("Une erreur est survenue.\nAction non définie.", data, "MainWindow");
			break;
		}
	}
}
