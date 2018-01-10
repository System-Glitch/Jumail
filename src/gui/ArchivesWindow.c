/**
 * 	ArchivesWindow.c
 *
 *  Author : Jérémy LAMBERT
 *  Date : 05/01/2018
 *  Description : Creates and manage the archive window
 */
#include "ArchivesWindow.h"
#include "MailWindow.h"

linkedlist_t *loaded_archived_mails = NULL;
linkedlist_t *loaded_archived_mails_paths = NULL;

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

/**
 * Gets the selected row in the browsing tree and puts the reference to the value of the row in "string"
 */
void tree_browsing_archives_get_selected_row(SGlobalData *data, gchar **string, GtkTreeIter *iter) {
	GtkTreeModel *model;
	GtkWidget *tree_view;
	GtkTreeSelection *selection;

	tree_view = GTK_WIDGET(gtk_builder_get_object (data->builder, "TreeViewBrowsingArchives"));
	model = gtk_tree_view_get_model (GTK_TREE_VIEW(tree_view));
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));

	if (gtk_tree_selection_count_selected_rows(selection)  == 1) {

		/* Get selected row */
		gtk_tree_selection_get_selected (selection, &model, iter);
		gtk_tree_model_get (model, iter, 0, string, -1);

	}
}

void callback_show_archives(GtkMenuItem *menuitem, gpointer user_data) {
	SGlobalData *data = (SGlobalData*) user_data;
	open_archives_window(data);
}

void callback_browsing_archives_delete (GtkMenuItem *menuitem, gpointer user_data) {
	SGlobalData *data = (SGlobalData*) user_data;
	action = DELETE_ARCHIVE_FOLDER;
	show_confirm_dialog("Êtes-vous sûr de vouloir supprimer ce dossier?\nCette action est irréversible.", data, "ArchivesWindow");
}

void callback_browsing_archives_create(GtkMenuItem *menuitem, gpointer user_data) {
	GtkWidget *dialog;
	GtkWidget *entry;
	gchar *string = NULL;
	char *string2; //Concat of string with "/dossier"
	GtkTreeIter iter;
	GtkWidget *button;
	SGlobalData *data = (SGlobalData*) user_data;
	action = CREATE_ARCHIVE_FOLDER;

	dialog = GTK_WIDGET(gtk_builder_get_object (data->builder, "CreateFolderDialog"));
	entry = GTK_WIDGET(gtk_builder_get_object (data->builder, "EntryFolderName"));
	button = GTK_WIDGET(gtk_builder_get_object (data->builder, "ButtonConfirmCreateFolder"));

	//Get selected row to auto-fill the entry
	tree_browsing_archives_get_selected_row(data, &string, &iter);
	if(string != NULL) { //A row is selected
		string2 = malloc(strlen(string)+1+8);
		if(string2 == NULL) {
			window_show_error("Une erreur est survenue.\nMémoire insuffisante.", data, "ArchivesWindow");
			return;
		}
		strcpy(string2, string);
		strcat(string2, FILE_SEPARATOR_STR);
		strcat(string2, "dossier");
		gtk_entry_set_text (GTK_ENTRY(entry), string2);
		gtk_widget_set_sensitive(button, 1); //Set the "confirm" button active because there is text in the entry already
	} else {
		gtk_widget_set_sensitive(button, 0);
	}

	gtk_widget_show_all (dialog);
	gtk_widget_grab_focus (entry);
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

	browsing_refresh_archives_folder(NULL, data);
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

gboolean callback_browsing_archives_context_menu(GtkWidget *tree_view, GdkEventButton *event, gpointer user_data) {
	SGlobalData *data = (SGlobalData*) user_data;
	GtkTreeModel *model = gtk_tree_view_get_model (GTK_TREE_VIEW(tree_view));
	gchar *string;
	GtkTreeIter iter;
	GtkWidget *menu;
	GtkWidget *menu_item_remove;

	menu = GTK_WIDGET(gtk_builder_get_object (data->builder, "ContextMenuBrowsingArchives"));
	menu_item_remove = GTK_WIDGET(gtk_builder_get_object (data->builder, "MenuBrowsingRemoveArchive"));


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
	return FALSE;
}

void callback_browsing_archives_refresh (GtkMenuItem *menuitem, gpointer user_data) {
	SGlobalData *data = (SGlobalData*) user_data;
	tree_browsing_archives_refresh(data);
}

/**
 * Gets all mails in the given folder and display on the GUI. Returns 1 on success, 0 on failure.
 */
int browsing_refresh_archives_folder(char * folder, SGlobalData *data) {
	GtkTreeIter iter;
	GtkListStore *model;
	GtkTreeView *tree_view;
	GtkTreeViewColumn *column;
	GtkCellRenderer *renderer;
	Email *mail;
	node_t *current;

	printf("Getting archive folder content : %s\n", folder);

	mail_window_clear(data);
	free_list_loaded_archived_mails();
	linkedlist_free(loaded_archived_mails_paths);

	loaded_archived_mails_paths = linkedlist_init();
	if(loaded_archived_mails_paths == NULL) {
		window_show_error("Mémoire insuffisante.", data, "ArchivesWindow");
		return 0;
	}
	loaded_archived_mails = linkedlist_init();
	if(loaded_archived_mails == NULL) {
		linkedlist_free(loaded_archived_mails_paths);
		window_show_error("Mémoire insuffisante.", data, "ArchivesWindow");
		return 0;
	}

	tree_view = GTK_TREE_VIEW (gtk_builder_get_object (data->builder, "TreeViewFolderListArchives"));
	if(tree_view == NULL) {
		window_show_error("Impossible de charger l'interface de dossier.", data, "MainWindow");
		return 0;
	}

	model = GTK_LIST_STORE(gtk_tree_view_get_model(tree_view));
	if(model == NULL) {

		model = gtk_list_store_new (4, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
		gtk_tree_view_set_model (tree_view, GTK_TREE_MODEL (model));

		renderer = gtk_cell_renderer_text_new ();
		column = gtk_tree_view_column_new_with_attributes ("Sujet", renderer, "text", 0, NULL);
		gtk_tree_view_column_set_resizable (column, TRUE);
		gtk_tree_view_append_column (tree_view, column);

		column = gtk_tree_view_column_new_with_attributes ("De", renderer, "text", 1, NULL);
		gtk_tree_view_column_set_resizable (column, TRUE);
		gtk_tree_view_append_column (tree_view, column);

		column = gtk_tree_view_column_new_with_attributes ("Pour", renderer, "text", 2, NULL);
		gtk_tree_view_column_set_resizable (column, TRUE);
		gtk_tree_view_append_column (tree_view, column);

		column = gtk_tree_view_column_new_with_attributes ("Date", renderer, "text", 3, NULL);
		gtk_tree_view_column_set_resizable (column, TRUE);
		gtk_tree_view_append_column (tree_view, column);
	}

	gtk_list_store_clear(model);

	if(folder != NULL) {
		if(archives_load_folder(loaded_archived_mails,loaded_archived_mails_paths,folder)) {
			current = loaded_archived_mails->head;
			while(current != NULL) {
				mail = current->val;
				if(mail == NULL) {
					window_show_error("Une erreur est survenue lors de la récupération des messages.", data, "ArchivesWindow");
					break;
				}
				gtk_list_store_append(model, &iter);

				gtk_list_store_set (model, &iter, 0, mail->subject, 1, mail->from, 2, mail->to, 3, mail->date, -1);
				iter.user_data = (gpointer)mail;
				current = current->next;
			}
		} else {
			free_list_loaded_archived_mails();
			window_show_error("Une erreur est survenue lors de la récupération des messages.", data, "ArchivesWindow");
		}
	}

	return 1;
}

void callback_show_archived_mail(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data) {
	char *mail_path;
	SGlobalData *data = (SGlobalData*) user_data;
	Email *mail;
	int *i = gtk_tree_path_get_indices ( path );

	mail_path = linkedlist_get(loaded_archived_mails_paths, *i);
	if(mail_path != NULL) {
		printf("%s\n", mail_path);
		mail = readEmailFile(mail_path);
		if(mail == NULL) {
			window_show_error("Une erreur est survenue.\nImpossible de récupérer le message.", data, "ArchivesWindow");
			return;
		}
		data->selected_mail_index = *i;
		if(!open_mail_window_from_file(mail ,data)) {
			window_show_error("Une erreur est survenue.\nImpossible de récupérer le message.", data, "ArchivesWindow");
		}
	}

}

void callback_browsing_archives_select(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data) {
	GtkTreeIter iter;
	SGlobalData *data = (SGlobalData*) user_data;
	GtkTreeModel *model = gtk_tree_view_get_model (tree_view);
	gtk_tree_model_get_iter(model, &iter, path);
	gtk_tree_model_get (model, &iter, 0, &data->selected_folder_archives, -1);

	browsing_refresh_archives_folder(data->selected_folder_archives, data);
}

/**
 * Returns the index of the selected row in the folder content view. Returns -1 if nothing is selected
 */
int archive_list_folder_get_selected_row(SGlobalData *data, GtkTreeIter *iter) {
	GtkTreeView *tree_view;
	GtkTreeSelection * tsel;
	GtkTreeModel * tm ;
	GtkTreePath * path ;
	int * i ;

	tree_view = GTK_TREE_VIEW(gtk_builder_get_object (data->builder, "TreeViewFolderListArchives"));

	tsel = gtk_tree_view_get_selection (tree_view);
	if ( gtk_tree_selection_get_selected ( tsel , &tm , iter ) ) {
		path = gtk_tree_model_get_path ( tm , iter ) ;
		i = gtk_tree_path_get_indices ( path ) ;
		return *i;
	}
	return -1;
}

gboolean callback_list_folder_archives_context_menu(GtkWidget *tree_view, GdkEventButton *event, gpointer user_data) {
	SGlobalData *data = (SGlobalData*) user_data;
	GtkTreeModel *model = gtk_tree_view_get_model (GTK_TREE_VIEW(tree_view));
	gchar *string;
	GtkTreeIter iter;
	GtkWidget *menu;

	menu = GTK_WIDGET(gtk_builder_get_object (data->builder, "ContextMenuMailsArchived"));


	if (event->type == GDK_BUTTON_PRESS  &&  event->button == 3) {

		GtkTreeSelection *selection;

		selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));

		if (gtk_tree_selection_count_selected_rows(selection)  <= 1) {
			GtkTreePath *path;

			/* Get tree path for row that was clicked */
			if (gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(tree_view),	(gint) event->x, (gint) event->y, &path, NULL, NULL, NULL)) {
				gtk_tree_model_get_iter(model, &iter, path);
				gtk_tree_model_get (model, &iter, 0, &string, -1);

				//Don't show popup menu if no mail is selected
				gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL, (event != NULL) ? event->button : 0,	gdk_event_get_time((GdkEvent*)event));
			}
		}
	}
	return FALSE;
}
void callback_archived_mail_move(GtkMenuItem *menuitem, gpointer user_data) {
	SGlobalData *data = (SGlobalData*) user_data;
	action = MOVE_ARCHIVED_MAIL;

	mail_window_clear(data);
	show_folder_select_dialog(data, "ArchivesWindow");
}

void callback_archived_mail_delete(GtkMenuItem *menuitem, gpointer user_data) {
	SGlobalData *data = (SGlobalData*) user_data;
	action = DELETE_ARCHIVED_MAIL;

	show_confirm_dialog("Êtes-vous sûr de vouloir supprimer ce message?\nCette action est irréversible.", data, "ArchivesWindow");
}
