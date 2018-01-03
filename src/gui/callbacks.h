/**
 * 	callbacks.h
 *
 *  Author : Jérémy LAMBERT
 *  Date : 02/01/2018
 *  Description : Holds all the callbacks for the GUI
 */

#ifndef SRC_CALLBACKS_H_
#define SRC_CALLBACKS_H_

#include <gtk/gtk.h>

typedef struct
{
	GtkBuilder *builder;
	gpointer user_data;
} SGlobalData;

enum Action {
	NONE,
	DELETE_FOLDER,
	CREATE_FOLDER,
	DELETE_MAIL
};

/**
 * Displays a modal confirm dialog and calls callback_confirm_response
 */
void show_confirm_dialog(const char * message, SGlobalData *data);

/**
 * Displays a modal error dialog.
 */
void window_show_error(const char * message, SGlobalData *data);

/**
 * Displays a modal success dialog.
 */
void window_show_info(const char * message, SGlobalData *data);

void callback_quit(GtkMenuItem *menuitem, gpointer user_data);
void callback_about (GtkMenuItem *menuitem, gpointer user_data);
void callback_browsing_select(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data);
void callback_show_mail(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data);
void callback_browsing_context_menu(GtkWidget *tree_view, GdkEventButton *event, gpointer user_data);
void callback_browsing_refresh (GtkMenuItem *menuitem, gpointer user_data);
void callback_browsing_delete (GtkMenuItem *menuitem, gpointer user_data);
void callback_confirm_response(GtkDialog *dialog, gint response_id, gpointer user_data);
void callback_browsing_create(GtkMenuItem *menuitem, gpointer user_data);
void callback_create_folder_confirm(GtkButton *widget, gpointer user_data);
void callback_create_folder_cancel(GtkButton *widget, gpointer user_data);
void callback_folder_create_entry_changed(GtkEditable *editable, gpointer user_data);
void callback_compose_mail(GtkMenuItem *menuitem, gpointer user_data);
void callback_compose_mail_to_entry_changed(GtkEditable *editable, gpointer user_data);
void callback_compose_mail_send(GtkToolButton *widget, gpointer user_data);
void callback_mail_delete(GtkMenuItem *menuitem, gpointer user_data);
void callback_list_folder_context_menu(GtkWidget *tree_view, GdkEventButton *event, gpointer user_data);

#endif /* SRC_CALLBACKS_H_ */
