/**
 * 	ArchivesWindow.c
 *
 *  Author : Jérémy LAMBERT
 *  Date : 05/01/2018
 *  Description : Creates and manage the archive window
 */
#include "ArchivesWindow.h"

linkedlist_t *loaded_archived_mails = NULL;

void free_list_loaded_archived_mails() {
	if(loaded_archived_mails == NULL)
		return;
	linkedlist_free(loaded_archived_mails);
}

void open_archives_window(SGlobalData *data) {
	GtkWidget *window = NULL;

	window =  GTK_WIDGET (gtk_builder_get_object (data->builder, "ArchivesWindow"));

	if(!gtk_widget_get_visible (window)) {
		gtk_widget_show_all (window);
		tree_browsing_archives_refresh(data);
	}
}

void callback_show_archives(GtkMenuItem *menuitem, gpointer user_data) {
	SGlobalData *data = (SGlobalData*) user_data;
	open_archives_window(data);
}

/**
 * Empty and load the tree store with the available folders.
 * Return 1 if success, 0 on failure. Triggers an error dialog.
 */
int tree_browsing_archives_refresh(SGlobalData *data) {

	GtkTreeIter iter;
	GtkTreeStore *model;
	GtkTreeView *tree_view;
	GtkTreeViewColumn *column;
	GtkCellRenderer *renderer;
	linkedlist_t *list;

	fputs("Refreshing archives browser\n", stdout);

	tree_view = GTK_TREE_VIEW (gtk_builder_get_object (data->builder, "TreeViewBrowsingArchives"));
	if(tree_view == NULL) {
		window_show_error("Impossible de charger l'interface de navigation.", data, "MainWindow");
		return 0;
	}
	model = GTK_TREE_STORE(gtk_tree_view_get_model(tree_view));

	//browsing_refresh_archives_folder(NULL, data);
	if(model != NULL) gtk_tree_store_clear(model);

	list = linkedlist_init();
	if(list == NULL) {
		window_show_error("Une erreur est survenue.\nMémoire insuffisante.", data, "ArchivesWindow");
		return 0;
	}

	list_archives_folders(list, ARCHIVES_FOLDER_NAME);

	if(list == NULL) {
		window_show_error("Impossible de charger les dossiers existants.\nVérifiez votre connexion internet et les paramètres de votre profil.", data, "ArchivesWindow");
		return 0;
	}

	if(model == NULL) {

		model = gtk_tree_store_new (1, G_TYPE_STRING);
		gtk_tree_view_set_model (tree_view, GTK_TREE_MODEL (model));

		renderer = gtk_cell_renderer_text_new ();
		column = gtk_tree_view_column_new_with_attributes ("Navigation", renderer, "text", 0 , NULL);
		gtk_tree_view_append_column (tree_view, column);
	} else {
		gtk_tree_store_clear(model);
	}

	for(size_t i = 0 ; i < list->length ; i++) {
		gtk_tree_store_append(model, &iter, NULL);
		gtk_tree_store_set (model, &iter, 0, (char*)linkedlist_get(list, i), -1);
	}

	linkedlist_free(list); //Always free

	return 1;
}
