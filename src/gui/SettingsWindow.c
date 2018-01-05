/**
 * 	SettingsWindow.c
 *
 *  Author : Jérémy LAMBERT
 *  Date : 05/01/2018
 *  Description : Creates and manage the settings window
 */
#include "SettingsWindow.h"
#include "../profils.h"

static gboolean clear_profile_selection_tree_view(GtkTreeModel *model, GtkTreePath  *path, GtkTreeIter *iter, gpointer user_data) {
	gtk_list_store_set(GTK_LIST_STORE(model), iter, 0, FALSE, -1);
	return FALSE;
}

void callback_profile_toggle(GtkCellRendererToggle *cell_renderer, gchar *path, gpointer user_data) {
	SGlobalData *data = (SGlobalData*) user_data;
	GtkListStore *model;
	GtkTreeView *tree_view;
	GtkTreePath *tree_path;
	gint *i;
	GtkTreeIter iter;

	tree_view = GTK_TREE_VIEW (gtk_builder_get_object (data->builder, "TreeViewProfiles"));
	model = GTK_LIST_STORE(gtk_tree_view_get_model(tree_view));
	tree_path = gtk_tree_path_new_from_string(path);
	i = gtk_tree_path_get_indices (tree_path);

	//Clear all selection
	gtk_tree_model_foreach (GTK_TREE_MODEL(model), clear_profile_selection_tree_view, NULL);

	//Set the selected one to "activated"
	gtk_tree_model_get_iter(GTK_TREE_MODEL(model), &iter, tree_path);
	gtk_list_store_set(model, &iter, 0, TRUE, -1);
	//printf("%d\n", *i);
	g_free(i);
}

void init_settings_window(SGlobalData *data) {
	GtkTreeIter iter;
	GtkListStore *model;
	GtkTreeView *tree_view;
	GtkTreeViewColumn *column;
	GtkCellRenderer *renderer;
	GtkCellRenderer *renderer_toggle;

	fputs("Initializing settings window...\n", stdout);

	tree_view = GTK_TREE_VIEW (gtk_builder_get_object (data->builder, "TreeViewProfiles"));
	if(tree_view == NULL) {
		window_show_error("Impossible de charger l'interface de préférences.", data, "MainWindow");
		return;
	}

	model = gtk_list_store_new (2, G_TYPE_BOOLEAN, G_TYPE_STRING);
	gtk_tree_view_set_model (tree_view, GTK_TREE_MODEL (model));

	renderer_toggle = gtk_cell_renderer_toggle_new ();
	gtk_cell_renderer_toggle_set_radio(GTK_CELL_RENDERER_TOGGLE(renderer_toggle), TRUE);
	column = gtk_tree_view_column_new_with_attributes ("Sélectionné", renderer_toggle, "active", 0 , NULL);
	g_signal_connect (renderer_toggle, "toggled", G_CALLBACK (callback_profile_toggle), data);
	gtk_tree_view_append_column (tree_view, column);

	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes ("Nom du profil", renderer, "text", 1 , NULL);
	gtk_tree_view_append_column (tree_view, column);

	if(listProfile == NULL)
		return;

	//Fill with existing profiles
	if(listProfile->length){
		node_t * current = listProfile->head;
		while(current != NULL){
			gtk_list_store_append(model, &iter);
			gtk_list_store_set (model, &iter, 0, FALSE, 1, ((Profile*)current->val)->nameOfProfile, -1);
			current = current->next;
		}
	}
}

void open_settings_window(SGlobalData *data) {
	GtkWidget *window = NULL;

	window =  GTK_WIDGET (gtk_builder_get_object (data->builder, "SettingsWindow"));

	if(!gtk_widget_get_visible (window)) {
		gtk_widget_show_all (window);
	}
}
