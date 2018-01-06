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
#include "SettingsWindow.h"

enum Action action = NONE;

char strequals(char* str, char* str2) {
	if(str == NULL || str2 == NULL) return 0;
	return !strcmp(str,str2);
}

char check_selected_profile() {
	return current_profile != NULL;
}

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
	free_email(data->response_reference);
	if(data->response_reference != NULL)
		free(data->response_reference);
	if(data->size != NULL)
		free(data->size);
}

void callback_confirm_response(GtkDialog *dialog, gint response_id, gpointer user_data) {
	SGlobalData *data = (SGlobalData*) user_data;
	GtkTreeStore *tree_store;
	GtkListStore *list_store;
	GtkWidget *tree_view;
	gchar *string;
	char *filename;
	GtkTreeIter iter;
	Email *mail;
	Profile *profile;
	int i;
	gint *index;

	if(response_id == -8) { //YES

		gtk_widget_hide(GTK_WIDGET(dialog));
		switch(action) {
		case DELETE_FOLDER:
			if(!check_selected_profile(data, "MainWindow")) return;
			mail_window_clear(data);
			tree_view = GTK_WIDGET(gtk_builder_get_object (data->builder, "TreeViewBrowsing"));
			tree_browsing_get_selected_row(data, &string, &iter);
			if(ssl_remove_folder(current_profile->emailAddress, current_profile->password, current_profile->receiveP, string, !strcmp(current_profile->SslImap, "TRUE"))) {
				window_show_error("Impossible de supprimer le dossier.\nVérifiez votre connexion internet et les paramètres de votre profil.", data, "MainWindow");
			} else {
				//Remove folder from GUI
				tree_store = GTK_TREE_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(tree_view)));
				gtk_tree_store_remove (tree_store, &iter);
				data->page = 0;
				browsing_refresh_folder(NULL, data);
			}
			break;
		case DELETE_PROFILE:
			tree_view = GTK_WIDGET(gtk_builder_get_object (data->builder, "TreeViewProfiles"));
			tree_profile_get_selected_row(data, &string, &iter);
			filename = malloc(strlen(PROFILE_FILENAME_START) + strlen(PROFILE_FILENAME_END) + strlen((char*)string) + 1);
			if(filename == NULL) {
				window_show_error("Une erreur est survenue.\nMémoire insuffisante.", data, "SettingsWindow");
				return;
			}
			strcpy(filename, PROFILE_FILENAME_START);
			strcat(filename, string);
			strcat(filename, PROFILE_FILENAME_END);
			if(remove(filename)) {
				window_show_error("Erreur. Impossible de supprimer le profil.", data, "SettingsWindow");
			} else {
				//Remove profile from loaded profiles
				list_store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(tree_view)));
				index = gtk_tree_path_get_indices(gtk_tree_model_get_path(GTK_TREE_MODEL(list_store), &iter));
				profile = linkedlist_get(listProfile, *index);
				if(profile == NULL) {
					window_show_error("Une erreur est survenue.\nLe profil sélectionné n'existe pas.", data, "SettingsWindow");
				} else {
					freeProfile(profile);
					linkedlist_remove_index(listProfile, *index);

					//Remove profile from GUI
					gtk_list_store_remove (list_store, &iter);
					settings_window_set_all_fields_active(data, FALSE);
					settings_window_fill_entries(data, NULL);
					gtk_tree_selection_unselect_all (gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view)));
				}
			}
			free(filename);
			break;
		case CREATE_FOLDER:
		case MOVE_MAIL:
		case MOVE_MAIL_FROM_VIEW:
		case RESPOND_MAIL_FROM_VIEW:
			window_show_error("Une erreur est survenue.\nAction invalide pour cette fonction.", data, "MainWindow");
			break;
		case DELETE_MAIL:
			if(!check_selected_profile(data, "MainWindow")) return;
			mail_window_clear(data);
			tree_view = GTK_WIDGET(gtk_builder_get_object (data->builder, "TreeViewFolderList"));
			i = list_folder_get_selected_row(data, &iter);
			if(i >= 0) {
				mail = linkedlist_get(loaded_mails, i);
				if(ssl_delete_mail(current_profile->emailAddress, current_profile->password, current_profile->receiveP, mail->mailbox ,mail->message_id, !strcmp(current_profile->SslImap, "TRUE")) != 0) {
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
			if(!check_selected_profile(data, "MailWindow")) return;
			i = data->selected_mail_index;
			mail_window_clear(data);
			tree_view = GTK_WIDGET(gtk_builder_get_object (data->builder, "TreeViewFolderList"));
			if(i >= 0) {
				mail = linkedlist_get(loaded_mails, i);
				if(ssl_delete_mail(current_profile->emailAddress, current_profile->password, current_profile->receiveP, mail->mailbox ,mail->message_id, !strcmp(current_profile->SslImap, "TRUE")) != 0) {
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

void callback_mail_move_confirm(GtkButton *widget, gpointer user_data) {
	SGlobalData *data = (SGlobalData*) user_data;
	GtkTreeModel *model;
	GtkListStore *list_store;
	GtkWidget *tree_view;
	GtkWidget *tree_view_mails;
	GtkTreeSelection *selection;
	gchar *folder_dst;
	GtkTreeIter iter;
	GtkWidget *dialog;
	int index = -1;
	int status = -1;
	int i = -1;
	Email *mail;

	if(!check_selected_profile(data, action == MOVE_MAIL ? "MainWindow" : "MailWindow")) return;

	index = list_folder_get_selected_row(data, &iter);
	if(index == -1) {
		window_show_error("Une erreur est survenue.\nAucun message n'est sélectionné.", data, "MainWindow");
		return;
	}

	dialog = GTK_WIDGET(gtk_builder_get_object (data->builder, "SelectFolderDialog"));
	tree_view = GTK_WIDGET(gtk_builder_get_object (data->builder, "TreeViewFolderPick"));
	tree_view_mails = GTK_WIDGET(gtk_builder_get_object (data->builder, "TreeViewFolderList"));
	model = gtk_tree_view_get_model (GTK_TREE_VIEW(tree_view));
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));

	if (gtk_tree_selection_count_selected_rows(selection)  == 1) {

		/* Get selected row for destination folder */
		gtk_tree_selection_get_selected (selection, &model, &iter);
		gtk_tree_model_get (model, &iter, 0, &folder_dst, -1);

		if(action == MOVE_MAIL)
			i = list_folder_get_selected_row(data, &iter); //Get selected mail
		else if(action == MOVE_MAIL_FROM_VIEW)
			i = data->selected_mail_index;
		if(i >= 0) {

			mail = linkedlist_get(loaded_mails, i);

			status = ssl_move_mail(current_profile->emailAddress, current_profile->password, current_profile->receiveP, mail->mailbox, folder_dst, mail->message_id, !strcmp(current_profile->SslImap, "TRUE"));

			if(status) {
				window_show_error("Impossible de déplacer le message.\nVérifiez votre connexion internet et les paramètres de votre profil.", data, action == MOVE_MAIL ? "MainWindow" : "MailWindow");
			} else {
				//Remove mail from GUI
				list_store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(tree_view_mails)));

				if(action == MOVE_MAIL_FROM_VIEW) {
					gtk_tree_model_get_iter_first(GTK_TREE_MODEL(list_store), &iter);
					for(int j = 0 ; j < i ; j++)
						gtk_tree_model_iter_next (GTK_TREE_MODEL(list_store), &iter);
				}

				gtk_list_store_remove (list_store, &iter);

				free_email(mail);
				linkedlist_remove_index(loaded_mails, i);
				gtk_widget_hide(dialog);
				if(action == MOVE_MAIL_FROM_VIEW) mail_window_clear(data);
				window_show_info("Le message a été déplacé.", data, "MainWindow");
			}
		} else {
			gtk_widget_hide(dialog);
			window_show_error("Une erreur est survenue.\nAucun message sélectionné.", data, action == MOVE_MAIL ? "MainWindow" : "MailWindow");
		}

	} else {
		gtk_widget_hide(dialog);
		window_show_error("Une erreur est survenue.\nAucun dossier n'est sélectionné.", data, action == MOVE_MAIL ? "MainWindow" : "MailWindow");
	}
}

static gboolean fill_folder_pick_view(GtkTreeModel *model, GtkTreePath  *path, GtkTreeIter *iter, gpointer user_data) {
	GtkTreeStore *model_pick;
	GtkTreeView *view;
	GtkTreeIter iter_pick;
	SGlobalData *data = (SGlobalData*) user_data;
	gchar *string;

	view = GTK_TREE_VIEW(gtk_builder_get_object (data->builder, "TreeViewFolderPick"));
	model_pick = GTK_TREE_STORE(gtk_tree_view_get_model(view));

	gtk_tree_model_get (model, iter, 0, &string, -1);
	gtk_tree_store_append(model_pick, &iter_pick, NULL);
	gtk_tree_store_set (model_pick, &iter_pick, 0, string, -1);

	return FALSE;
}

void show_folder_select_dialog(SGlobalData *data, char *parent_window_name) {
	GtkWidget *dialog;
	GtkWidget *parent;
	GtkTreeStore *model;
	GtkTreeStore *model_browsing;
	GtkTreeView *view;
	GtkTreeView *view_browsing;
	GtkTreeViewColumn *column;
	GtkCellRenderer *renderer;


	dialog = GTK_WIDGET(gtk_builder_get_object (data->builder, "SelectFolderDialog"));
	parent = GTK_WIDGET(gtk_builder_get_object (data->builder, parent_window_name));
	view = GTK_TREE_VIEW(gtk_builder_get_object (data->builder, "TreeViewFolderPick"));
	view_browsing = GTK_TREE_VIEW(gtk_builder_get_object (data->builder, "TreeViewBrowsing"));

	gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(parent));

	model = GTK_TREE_STORE(gtk_tree_view_get_model(view));
	model_browsing = GTK_TREE_STORE(gtk_tree_view_get_model(view_browsing));
	if(model == NULL) {

		model = gtk_tree_store_new (1, G_TYPE_STRING);
		gtk_tree_view_set_model (view, GTK_TREE_MODEL (model));

		renderer = gtk_cell_renderer_text_new ();
		column = gtk_tree_view_column_new_with_attributes ("Nom du dossier", renderer, "text", 0 , NULL);
		gtk_tree_view_append_column (view, column);
	}

	gtk_tree_store_clear(model);

	gtk_tree_model_foreach(GTK_TREE_MODEL(model_browsing), fill_folder_pick_view, data);

	gtk_widget_show_all (dialog);
}
