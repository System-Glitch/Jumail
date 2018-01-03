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
			}
		}

		gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL, (event != NULL) ? event->button : 0,	gdk_event_get_time((GdkEvent*)event));
	}
}

void callback_browsing_refresh (GtkMenuItem *menuitem, gpointer user_data) {
	SGlobalData *data = (SGlobalData*) user_data;
	tree_browsing_refresh(data);
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
