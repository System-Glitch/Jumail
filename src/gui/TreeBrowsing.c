/**
 * 	TreeBrowsing.c
 *
 *  Author : Jérémy LAMBERT
 *  Date : 02/01/2018
 *  Description : Handler for the folder navigation menu
 */
#include <stdlib.h>
#include <gtk/gtk.h>
#include "../mailing.h"
#include "callbacks.h"
#include "TreeBrowsing.h"
#include "MailWindow.h"

/**
 * Gets the selected row in the browsing tree and puts the reference to the value of the row in "string"
 */
void tree_browsing_get_selected_row(SGlobalData *data, gchar **string, GtkTreeIter *iter) {
	GtkTreeModel *model;
	GtkWidget *tree_view;
	GtkTreeSelection *selection;

	tree_view = GTK_WIDGET(gtk_builder_get_object (data->builder, "TreeViewBrowsing"));
	model = gtk_tree_view_get_model (GTK_TREE_VIEW(tree_view));
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));

	if (gtk_tree_selection_count_selected_rows(selection)  == 1) {

		/* Get selected row */
		gtk_tree_selection_get_selected (selection, &model, iter);
		gtk_tree_model_get (model, iter, 0, string, -1);

	}
}

/**
 * Empty and load the tree store with the available folders. Safely checks if a profile is selected or not.
 * Return 1 if success, 0 on failure. Triggers an error dialog.
 */
int tree_browsing_refresh(SGlobalData *data) {

	GtkTreeIter iter;
	GtkTreeStore *model;
	GtkTreeView *tree_view;
	GtkTreeViewColumn *column;
	GtkCellRenderer *renderer;

	fputs("Refreshing browser\n", stdout);

	//TODO check selected profile

	tree_view = GTK_TREE_VIEW (gtk_builder_get_object (data->builder, "TreeViewBrowsing"));
	if(tree_view == NULL) {
		window_show_error("Impossible de charger l'interface de navigation.", data);
		return 0;
	}

	StringArray *list = ssl_list("jumailimap@gmail.com", "azerty12", "imap.gmail.com");

	if(list == NULL) {
		window_show_error("Impossible de charger les dossiers existants.\nVérifiez votre connexion internet et les paramètres de votre profil.", data);
		return 0;
	}

	model = GTK_TREE_STORE(gtk_tree_view_get_model(tree_view));
	if(model == NULL) {

		model = gtk_tree_store_new (1, G_TYPE_STRING);
		gtk_tree_view_set_model (tree_view, GTK_TREE_MODEL (model));

		renderer = gtk_cell_renderer_text_new ();
		column = gtk_tree_view_column_new_with_attributes ("Navigation", renderer, "text", 0, NULL);
		gtk_tree_view_append_column (tree_view, column);
	} else {
		gtk_tree_store_clear(model);
	}

	for(size_t i = 0 ; i < list->size ; i++) {
		gtk_tree_store_append(model, &iter, NULL);
		gtk_tree_store_set (model, &iter, 0, list->array[i], -1);
	}

	free_string_array(*list); //Always free
	free(list);

	return 1;
}

/**
 * Gets all mails in the given folder and display on the GUI. Returns 1 on success, 0 on failure.
 */
int browsing_refresh_folder(char * folder, SGlobalData *data) {
	GtkTreeIter iter;
	GtkListStore *model;
	GtkTreeView *tree_view;
	GtkTreeViewColumn *column;
	GtkCellRenderer *renderer;

	printf("Getting folder content : %s\n", folder);

	mail_window_clear(data);
	free_list_loaded_mails();
	loaded_mails = linkedlist_init();
	if(loaded_mails == NULL) {
		window_show_error("Mémoire insuffisante.", data);
		return 0;
	}

	tree_view = GTK_TREE_VIEW (gtk_builder_get_object (data->builder, "TreeViewFolderList"));
	if(tree_view == NULL) {
		window_show_error("Impossible de charger l'interface de dossier.", data);
		return 0;
	}

	model = GTK_LIST_STORE(gtk_tree_view_get_model(tree_view));
	if(model == NULL) {

		model = gtk_list_store_new (4, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
		gtk_tree_view_set_model (tree_view, GTK_TREE_MODEL (model));

		renderer = gtk_cell_renderer_text_new ();
		column = gtk_tree_view_column_new_with_attributes ("Sujet", renderer, "text", 0, NULL);
		gtk_tree_view_append_column (tree_view, column);

		column = gtk_tree_view_column_new_with_attributes ("De", renderer, "text", 1, NULL);
		gtk_tree_view_append_column (tree_view, column);

		column = gtk_tree_view_column_new_with_attributes ("Pour", renderer, "text", 2, NULL);
		gtk_tree_view_append_column (tree_view, column);

		column = gtk_tree_view_column_new_with_attributes ("Date", renderer, "text", 3, NULL);
		gtk_tree_view_append_column (tree_view, column);
	} else {
		gtk_list_store_clear(model);
	}

	if(folder != NULL) {
		struct ParsedSearch *search = ssl_search_all("jumailimap@gmail.com", "azerty12", "imap.gmail.com", folder);
		if(search == NULL) {
			window_show_error("Impossible de charger le contenu du dossier.\nVérifiez votre connexion internet et les paramètres de votre profil.", data);
			return 1;
		}
		for(size_t i = search->size-1 ; i > 0; i--) {
			Email *mail = ssl_get_mail("jumailimap@gmail.com", "azerty12", "imap.gmail.com", folder, search->uids[i]);

			gtk_list_store_append(model, &iter);
			gtk_list_store_set (model, &iter, 0, mail->subject, 1, mail->from, 2, mail->to, 3, mail->date, -1);
			iter.user_data = (gpointer)mail;

			linkedlist_add(loaded_mails, mail);
		}
		free_parsed_search(search); //Always free
	}

	return 1;
}
