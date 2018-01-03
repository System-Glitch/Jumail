/**
 * 	callbacks.c
 *
 *  Author : Jérémy LAMBERT
 *  Date : 02/01/2018
 *  Description : Holds all the callbacks for the GUI
 */
#include <stdio.h>
#include "callbacks.h"
#include "../mailing.h"
#include "../folders.h"
#include "TreeBrowsing.h"
#include "MailWindow.h"

static enum Action action = NONE;

/**
 * Displays a modal confirm dialog and calls callback_confirm_response
 */
void show_confirm_dialog(const char * message, SGlobalData *data) {
	GtkWidget *dialog;
	GtkWidget *main_window;

	main_window = GTK_WIDGET(gtk_builder_get_object (data->builder, "MainWindow"));

	dialog = gtk_message_dialog_new (GTK_WINDOW(main_window), GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING, GTK_BUTTONS_YES_NO, "%s", message);
	g_signal_connect (dialog, "response", G_CALLBACK (callback_confirm_response), data);
	gtk_dialog_run (GTK_DIALOG(dialog));
	gtk_widget_destroy (dialog);
}

/**
 * Displays a modal error dialog.
 */
void window_show_error(const char * message, SGlobalData *data) {
	GtkWidget *dialog;
	GtkWidget *main_window;

	main_window = GTK_WIDGET(gtk_builder_get_object (data->builder, "MainWindow"));

	dialog = gtk_message_dialog_new (GTK_WINDOW(main_window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "%s", message);
	gtk_dialog_run (GTK_DIALOG(dialog));
	gtk_widget_destroy (dialog);
}

void callback_quit(GtkMenuItem *menuitem, gpointer user_data) {
	fputs("Closing GUI...\n", stdout);
	gtk_main_quit();
}

void callback_about (GtkMenuItem *menuitem, gpointer user_data) {
	SGlobalData *data = (SGlobalData*) user_data;
	GtkWidget *dialog = NULL;

	dialog =  GTK_WIDGET (gtk_builder_get_object (data->builder, "AboutDialog"));
	gtk_dialog_run (GTK_DIALOG (dialog));

	gtk_widget_hide (dialog);
}

void callback_browsing_select(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data) {
	gchar *string;
	GtkTreeIter iter;
	SGlobalData *data = (SGlobalData*) user_data;
	GtkTreeModel *model = gtk_tree_view_get_model (tree_view);
	gtk_tree_model_get_iter(model, &iter, path);
	gtk_tree_model_get (model, &iter, 0, &string, -1);
	browsing_refresh_folder(string, data);
}

void callback_browsing_context_menu(GtkWidget *tree_view, GdkEventButton *event, gpointer user_data) {
	SGlobalData *data = (SGlobalData*) user_data;
	GtkTreeModel *model = gtk_tree_view_get_model (GTK_TREE_VIEW(tree_view));
	gchar *string;
	GtkTreeIter iter;
	GtkWidget *menu;
	GtkWidget *menu_item_remove;

	menu = GTK_WIDGET(gtk_builder_get_object (data->builder, "ContextMenuBrowsing"));
	menu_item_remove = GTK_WIDGET(gtk_builder_get_object (data->builder, "MenuBrowsingRemove"));

	gtk_widget_set_sensitive(menu_item_remove, 0);

	if (event->type == GDK_BUTTON_PRESS  &&  event->button == 3) {

		GtkTreeSelection *selection;

		selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));

		if (gtk_tree_selection_count_selected_rows(selection)  <= 1) {
			GtkTreePath *path;

			/* Get tree path for row that was clicked */
			if (gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(tree_view),	(gint) event->x, (gint) event->y, &path, NULL, NULL, NULL)) {
				gtk_tree_model_get_iter(model, &iter, path);
				gtk_tree_model_get (model, &iter, 0, &string, -1);

				gtk_widget_set_sensitive(menu_item_remove, 1); //Enable "Delete" button if a row is selected
			} else {
				gtk_tree_selection_unselect_all(selection);
			}
		}

		gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL, (event != NULL) ? event->button : 0,	gdk_event_get_time((GdkEvent*)event));
	}
}

void callback_folder_create_entry_changed(GtkEditable *editable, gpointer user_data) {
	SGlobalData *data = (SGlobalData*) user_data;
	GtkWidget *button;

	button = GTK_WIDGET(gtk_builder_get_object (data->builder, "ButtonConfirmCreateFolder"));

	//Disable the "confirm" button if the field is empty
	gtk_widget_set_sensitive(button, strlen(gtk_entry_get_text(GTK_ENTRY(editable))));

}

void callback_browsing_refresh (GtkMenuItem *menuitem, gpointer user_data) {
	SGlobalData *data = (SGlobalData*) user_data;
	tree_browsing_refresh(data);
}

void callback_browsing_create(GtkMenuItem *menuitem, gpointer user_data) {
	GtkWidget *dialog;
	GtkWidget *entry;
	gchar *string = NULL;
	char *string2; //Concat of string with "/dossier"
	GtkTreeIter iter;
	GtkWidget *button;
	SGlobalData *data = (SGlobalData*) user_data;
	action = CREATE_FOLDER;

	dialog = GTK_WIDGET(gtk_builder_get_object (data->builder, "CreateFolderDialog"));
	entry = GTK_WIDGET(gtk_builder_get_object (data->builder, "EntryFolderName"));
	button = GTK_WIDGET(gtk_builder_get_object (data->builder, "ButtonConfirmCreateFolder"));

	//Get selected row to auto-fill the entry
	tree_browsing_get_selected_row(data, &string, &iter);
	if(string != NULL) { //A row is selected
		string2 = malloc(strlen(string)+1+8);
		if(string2 == NULL) {
			window_show_error("Une erreur est survenue.\nMémoire insuffisante.", data);
			return;
		}
		strcpy(string2, string);
		strcat(string2, "/dossier");
		gtk_entry_set_text (GTK_ENTRY(entry), string2);
		gtk_widget_set_sensitive(button, 1); //Set the "confirm" button active because there is text in the entry already
	} else {
		gtk_widget_set_sensitive(button, 0);
	}

	gtk_widget_show_all (dialog);
	gtk_widget_grab_focus (entry);
}

void callback_create_folder_confirm(GtkButton *widget, gpointer user_data) {
	GtkWidget *dialog;
	GtkWidget *entry;
	GtkTreeStore *model;
	GtkWidget *tree_view;
	GtkTreeIter iter;
	int status;
	const gchar *foldername;
	SGlobalData *data = (SGlobalData*) user_data;

	dialog = GTK_WIDGET(gtk_builder_get_object (data->builder, "CreateFolderDialog"));
	entry = GTK_WIDGET(gtk_builder_get_object (data->builder, "EntryFolderName"));
	tree_view = GTK_WIDGET(gtk_builder_get_object (data->builder, "TreeViewBrowsing"));
	model = GTK_TREE_STORE(gtk_tree_view_get_model (GTK_TREE_VIEW(tree_view)));
	foldername = gtk_entry_get_text(GTK_ENTRY(entry));

	status = ssl_create_folder("jumailimap@gmail.com", "azerty12", "imap.gmail.com", (char*)foldername); //TODO profile

	gtk_widget_hide(dialog);

	if(status != 0) {
		window_show_error("La création du dossier a échoué.", data);
	} else {
		gtk_tree_store_append(model, &iter, NULL);
		gtk_tree_store_set (model, &iter, 0, foldername, -1);
	}
	gtk_entry_set_text (GTK_ENTRY(entry), ""); //Clear text entry
}

void callback_create_folder_cancel(GtkButton *widget, gpointer user_data) {
	GtkWidget *dialog;
	GtkWidget *entry;
	SGlobalData *data = (SGlobalData*) user_data;
	dialog = GTK_WIDGET(gtk_builder_get_object (data->builder, "CreateFolderDialog"));
	entry = GTK_WIDGET(gtk_builder_get_object (data->builder, "EntryFolderName"));
	gtk_entry_set_text (GTK_ENTRY(entry), ""); //Clear text entry

	gtk_widget_hide(dialog);
}

void callback_browsing_delete (GtkMenuItem *menuitem, gpointer user_data) {
	SGlobalData *data = (SGlobalData*) user_data;
	action = DELETE_FOLDER;
	show_confirm_dialog("Êtes-vous sûr de vouloir supprimer ce dossier?\nCette action est irréversible.", data);
}

void callback_confirm_response(GtkDialog *dialog, gint response_id, gpointer user_data) {
	SGlobalData *data = (SGlobalData*) user_data;
	GtkTreeStore *tree_store;
	GtkWidget *tree_view;
	gchar *string;
	GtkTreeIter iter;

	if(response_id == -8) { //YES

		switch(action) {
		case DELETE_FOLDER:
			tree_view = GTK_WIDGET(gtk_builder_get_object (data->builder, "TreeViewBrowsing"));
			tree_browsing_get_selected_row(data, &string, &iter);
			if(ssl_remove_folder("jumailimap@gmail.com", "azerty12", "imap.gmail.com", string)) { //TODO profile
				window_show_error("Impossible de supprimer le dossier.\nVérifiez votre connexion internet et les paramètres de votre profil.", data);
			} else {
				//Remove folder from GUI
				tree_store = GTK_TREE_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(tree_view)));
				gtk_tree_store_remove (tree_store, &iter);
				browsing_refresh_folder(NULL, data);
			}
			break;
		case CREATE_FOLDER:
			window_show_error("Une erreur est survenue.\nAction invalide pour cette fonction.", data);
			break;
		case NONE:
			window_show_error("Une erreur est survenue.\nAction non définie.", data);
			break;
		}
	}
}

void callback_show_mail(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data) {
	Email *mail;
	SGlobalData *data = (SGlobalData*) user_data;
	int *i = gtk_tree_path_get_indices ( path );

	mail = linkedlist_get(loaded_mails, *i);
	if(mail != NULL) {
		open_mail_window(mail, data);
	}

}
