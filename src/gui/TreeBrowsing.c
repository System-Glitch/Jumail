/**
 * 	TreeBrowsing.c
 *
 *  Author : Jérémy LAMBERT
 *  Date : 02/01/2018
 *  Description : Handler for the folder navigation menu
 */
#include <stdlib.h>
#include "../mailing.h"
#include "TreeBrowsing.h"
#include "MailWindow.h"
#include "MainWindow.h"

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
 * Returns the index of the selected row in the folder content view. Returns -1 if nothing is selected
 */
int list_folder_get_selected_row(SGlobalData *data, GtkTreeIter *iter) {
	GtkTreeView *tree_view;
	GtkTreeSelection * tsel;
	GtkTreeModel * tm ;
	GtkTreePath * path ;
	int * i ;

	tree_view = GTK_TREE_VIEW(gtk_builder_get_object (data->builder, "TreeViewFolderList"));

	tsel = gtk_tree_view_get_selection (tree_view);
	if ( gtk_tree_selection_get_selected ( tsel , &tm , iter ) ) {
		path = gtk_tree_model_get_path ( tm , iter ) ;
		i = gtk_tree_path_get_indices ( path ) ;
		return *i;
	}
	return -1;
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

	tree_view = GTK_TREE_VIEW (gtk_builder_get_object (data->builder, "TreeViewBrowsing"));
	if(tree_view == NULL) {
		window_show_error("Impossible de charger l'interface de navigation.", data, "MainWindow");
		return 0;
	}
	model = GTK_TREE_STORE(gtk_tree_view_get_model(tree_view));

	data->page = 0;
	browsing_refresh_folder(NULL, data);
	if(model != NULL) gtk_tree_store_clear(model);
	if(!check_selected_profile(data, "MainWindow")) {
		return 0;
	}

	StringArray *list = ssl_list(current_profile->emailAddress, current_profile->password, current_profile->receiveP, strequals(current_profile->SslImap, "TRUE"));

	if(list == NULL) {
		window_show_error("Impossible de charger les dossiers existants.\nVérifiez votre connexion internet et les paramètres de votre profil.", data, "MainWindow");
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
	Email *mail;
	node_t *current;
	char pageStr[100];
	char* title;

	printf("Getting folder content : %s (Page %d)\n", folder, data->page+1);

	mail_window_clear(data);
	free_list_loaded_mails();
	gtk_window_set_title(GTK_WINDOW(gtk_builder_get_object (data->builder, "MainWindow")), "Jumail");

	loaded_mails = linkedlist_init();
	if(loaded_mails == NULL) {
		window_show_error("Mémoire insuffisante.", data, "MainWindow");
		return 0;
	}

	tree_view = GTK_TREE_VIEW (gtk_builder_get_object (data->builder, "TreeViewFolderList"));
	if(tree_view == NULL) {
		window_show_error("Impossible de charger l'interface de dossier.", data, "MainWindow");
		return 0;
	}

	model = GTK_LIST_STORE(gtk_tree_view_get_model(tree_view));
	if(model == NULL) {

		model = gtk_list_store_new (6, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, G_TYPE_BOOLEAN);
		gtk_tree_view_set_model (tree_view, GTK_TREE_MODEL (model));

		renderer = gtk_cell_renderer_text_new ();
		column = gtk_tree_view_column_new_with_attributes ("Sujet", renderer, "text", 0 , "weight", 4, "weight-set", 5, NULL);
		gtk_tree_view_column_set_resizable (column, TRUE);
		gtk_tree_view_append_column (tree_view, column);

		column = gtk_tree_view_column_new_with_attributes ("De", renderer, "text", 1, "weight", 4, "weight-set", 5, NULL);
		gtk_tree_view_column_set_resizable (column, TRUE);
		gtk_tree_view_append_column (tree_view, column);

		column = gtk_tree_view_column_new_with_attributes ("Pour", renderer, "text", 2, "weight", 4, "weight-set", 5, NULL);
		gtk_tree_view_column_set_resizable (column, TRUE);
		gtk_tree_view_append_column (tree_view, column);

		column = gtk_tree_view_column_new_with_attributes ("Date", renderer, "text", 3, "weight", 4, "weight-set", 5, NULL);
		gtk_tree_view_column_set_resizable (column, TRUE);
		gtk_tree_view_append_column (tree_view, column);
	}

	gtk_list_store_clear(model);

	if(folder != NULL) {
		*data->size = 0;
		if(ssl_load_mail_headers(current_profile->emailAddress, current_profile->password, current_profile->receiveP, folder, strequals(current_profile->SslImap, "TRUE"), data->page, data->size)) {
			current = loaded_mails->head;
			while(current != NULL) {
				mail = current->val;
				if(mail == NULL) {
					window_show_error("Une erreur est survenue lors de la récupération des messages.", data, "MainWindow");
					break;
				}
				gtk_list_store_append(model, &iter);

				if(!string_array_contains(mail->flags, "\\Seen"))
					gtk_list_store_set (model, &iter, 0, mail->subject, 1, mail->from, 2, mail->to, 3, mail->date, 4, PANGO_WEIGHT_BOLD, 5, TRUE, -1);
				else
					gtk_list_store_set (model, &iter, 0, mail->subject, 1, mail->from, 2, mail->to, 3, mail->date, -1);
				iter.user_data = (gpointer)mail;
				current = current->next;
			}

			//Update paginate
			sprintf(pageStr, "Page %d/%d", data->page+1, *data->size/MAX_MAIL_PER_PAGE+1);
			gtk_label_set_text(GTK_LABEL(gtk_builder_get_object (data->builder, "PageLabel")), pageStr);
			gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object (data->builder, "ButtonPagePrevious")), data->page > 0);
			gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object (data->builder, "ButtonPageNext")), (data->page+1)*MAX_MAIL_PER_PAGE < *data->size);

			title = malloc(9+strlen(data->selected_folder)+1);
			if(title != NULL) {
				strcpy(title, "Jumail - ");
				strcat(title, data->selected_folder);
				gtk_window_set_title(GTK_WINDOW(gtk_builder_get_object (data->builder, "MainWindow")), title);
				free(title);
			} else {
				window_show_error("Une erreur est survenue.\nMémoire insuffisante", data, "MainWindow");
			}
		} else {
			window_show_error("Une erreur est survenue lors de la récupération des messages.", data, "MainWindow");
		}
	} else {
		gtk_label_set_text(GTK_LABEL(gtk_builder_get_object (data->builder, "PageLabel")), "Page 1/1");
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object (data->builder, "ButtonPagePrevious")), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object (data->builder, "ButtonPageNext")), FALSE);
	}

	return 1;
}

gboolean callback_list_folder_context_menu(GtkWidget *tree_view, GdkEventButton *event, gpointer user_data) {
	SGlobalData *data = (SGlobalData*) user_data;
	GtkTreeModel *model = gtk_tree_view_get_model (GTK_TREE_VIEW(tree_view));
	gchar *string;
	GtkTreeIter iter;
	GtkWidget *menu;

	menu = GTK_WIDGET(gtk_builder_get_object (data->builder, "ContextMenuMails"));


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

void callback_mail_delete(GtkMenuItem *menuitem, gpointer user_data) {
	SGlobalData *data = (SGlobalData*) user_data;
	action = DELETE_MAIL;
	show_confirm_dialog("Êtes-vous sûr de vouloir supprimer ce message?\nCette action est irréversible.", data, "MainWindow");
}

static void mail_set_seen(SGlobalData* data, gboolean seen) {
	Email *mail;
	GtkTreeModel *model;
	GtkWidget *tree_view;
	GtkTreeIter iter;
	gchar *string;

	tree_view = GTK_WIDGET(gtk_builder_get_object (data->builder, "TreeViewFolderList"));
	model = gtk_tree_view_get_model (GTK_TREE_VIEW(tree_view));

	tree_browsing_get_selected_row(data, &string, &iter);
	int i = list_folder_get_selected_row(data, &iter);
	if(i >= 0) {

		mail = linkedlist_get(loaded_mails, i);
		if(ssl_see_mail(current_profile->emailAddress, current_profile->password, current_profile->receiveP, string, mail->message_id, strequals(current_profile->SslImap, "TRUE"), seen) != 0) {
			window_show_error("Impossible de changer le status du message.\nVérifiez votre connexion internet et les paramètres de votre profil.", data, "MainWindow");
		} else {
			if(!seen) {
				gtk_list_store_set (GTK_LIST_STORE(model), &iter, 0, mail->subject, 1, mail->from, 2, mail->to, 3, mail->date, 4, PANGO_WEIGHT_BOLD, 5, TRUE, -1);
			} else {
				gtk_list_store_set (GTK_LIST_STORE(model), &iter, 0, mail->subject, 1, mail->from, 2, mail->to, 3, mail->date, 4, PANGO_WEIGHT_NORMAL, 5, TRUE, -1);
			}
		}
	} else {
		window_show_error("Une erreur est survenue.\nAucun message sélectionné.", data, "MainWindow");
	}
}

void callback_mail_seen(GtkMenuItem *menuitem, gpointer user_data) {
	SGlobalData *data = (SGlobalData*) user_data;
	mail_set_seen(data, 1);
}

void callback_mail_unseen(GtkMenuItem *menuitem, gpointer user_data) {
	SGlobalData *data = (SGlobalData*) user_data;
	mail_set_seen(data, 0);
}

void callback_browsing_select(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data) {
	GtkTreeIter iter;
	SGlobalData *data = (SGlobalData*) user_data;
	GtkTreeModel *model = gtk_tree_view_get_model (tree_view);
	gtk_tree_model_get_iter(model, &iter, path);
	gtk_tree_model_get (model, &iter, 0, &data->selected_folder, -1);
	data->page = 0;

	browsing_refresh_folder(data->selected_folder, data);
}

gboolean callback_browsing_context_menu(GtkWidget *tree_view, GdkEventButton *event, gpointer user_data) {
	SGlobalData *data = (SGlobalData*) user_data;
	GtkTreeModel *model = gtk_tree_view_get_model (GTK_TREE_VIEW(tree_view));
	gchar *string;
	GtkTreeIter iter;
	GtkWidget *menu;
	GtkWidget *menu_item_remove;
	GtkWidget *menu_item_create;

	menu = GTK_WIDGET(gtk_builder_get_object (data->builder, "ContextMenuBrowsing"));
	menu_item_remove = GTK_WIDGET(gtk_builder_get_object (data->builder, "MenuBrowsingRemove"));
	menu_item_create = GTK_WIDGET(gtk_builder_get_object (data->builder, "MenuBrowsingCreate"));

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

		gtk_widget_set_sensitive(menu_item_create, check_selected_profile());

		gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL, (event != NULL) ? event->button : 0,	gdk_event_get_time((GdkEvent*)event));
	}
	return FALSE;
}

void callback_browsing_delete (GtkMenuItem *menuitem, gpointer user_data) {
	SGlobalData *data = (SGlobalData*) user_data;
	action = DELETE_FOLDER;
	show_confirm_dialog("Êtes-vous sûr de vouloir supprimer ce dossier?\nCette action est irréversible.", data, "MainWindow");
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
			window_show_error("Une erreur est survenue.\nMémoire insuffisante.", data, "MainWindow");
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

void callback_move_mail_cancel(GtkButton *widget, gpointer user_data) {
	SGlobalData *data = (SGlobalData*) user_data;
	GtkWidget *dialog;
	dialog = GTK_WIDGET(gtk_builder_get_object (data->builder, "SelectFolderDialog"));
	gtk_widget_hide(dialog);
}

void callback_mail_move(GtkMenuItem *menuitem, gpointer user_data) {
	SGlobalData *data = (SGlobalData*) user_data;
	action = MOVE_MAIL;

	mail_window_clear(data);
	show_folder_select_dialog(data, "MainWindow");
}

void callback_page_previous(GtkButton *widget, gpointer user_data) {
	SGlobalData *data = (SGlobalData*) user_data;

	if(data->page > 0)
		data->page--;

	browsing_refresh_folder(data->selected_folder, data);
}

void callback_page_next(GtkButton *widget, gpointer user_data) {
	SGlobalData *data = (SGlobalData*) user_data;
	data->page++;

	browsing_refresh_folder(data->selected_folder, data);

}

void callback_mail_archive(GtkMenuItem *menuitem, gpointer user_data) {
	SGlobalData *data = (SGlobalData*) user_data;
	action = ARCHIVE_MAIL;

	mail_window_clear(data);
	show_folder_select_dialog(data, "MainWindow");
}
