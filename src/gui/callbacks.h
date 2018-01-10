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
#include "../mailing.h"
#include "../profils.h"
#include "../Archive.h"

typedef struct
{
	GtkBuilder *builder;
	gpointer user_data;
	Email *current_email;
	Email *response_reference;
	int selected_mail_index;
	int selected_profile_index;
	int page;
	unsigned int *size;
	gchar *selected_folder;
	gchar *selected_folder_archives;
	gboolean is_archived;
} SGlobalData;

enum Action {
	NONE,
	DELETE_FOLDER,
	CREATE_FOLDER,
	DELETE_MAIL,
	DELETE_MAIL_FROM_VIEW,
	MOVE_MAIL_FROM_VIEW,
	RESPOND_MAIL_FROM_VIEW,
	DELETE_PROFILE,
	MOVE_MAIL,
	DELETE_ARCHIVE_FOLDER,
	CREATE_ARCHIVE_FOLDER,
	ARCHIVE_MAIL
};

extern enum Action action;

char strequals(char* str, char* str2);
char check_selected_profile();

/**
 * Displays a modal confirm dialog and calls callback_confirm_response
 */
void show_confirm_dialog(const char * message, SGlobalData *data, char *parent_window_name);

/**
 * Displays a modal error dialog.
 */
void window_show_error(const char * message, SGlobalData *data, char *parent_window_name);

/**
 * Displays a modal success dialog.
 */
void window_show_info(const char * message, SGlobalData *data, char *parent_window_name);

void show_folder_select_dialog(SGlobalData *data, char *parent_window_name);

void callback_quit(GtkMenuItem *menuitem, gpointer user_data);
void callback_about (GtkMenuItem *menuitem, gpointer user_data);
void callback_browsing_select(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data);
void callback_show_mail(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data);
gboolean callback_browsing_context_menu(GtkWidget *tree_view, GdkEventButton *event, gpointer user_data);
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
gboolean callback_list_folder_context_menu(GtkWidget *tree_view, GdkEventButton *event, gpointer user_data);
void callback_mail_seen(GtkMenuItem *menuitem, gpointer user_data);
void callback_mail_unseen(GtkMenuItem *menuitem, gpointer user_data);
void callback_mail_move(GtkMenuItem *menuitem, gpointer user_data);
void callback_mail_move_confirm(GtkButton *widget, gpointer user_data);
void callback_mail_window_close(GtkWidget *widget, GdkEvent *event, gpointer user_data);
void callback_mail_view_delete_email(GtkButton *widget, gpointer user_data);
void callback_mail_view_response(GtkButton *widget, gpointer user_data);
void callback_mail_compose_window_close(GtkWidget *widget, GdkEvent *event, gpointer user_data);
void callback_mail_view_see_raw(GtkButton *widget, gpointer user_data);
void callback_settings(GtkMenuItem *menuitem, gpointer user_data);
void callback_profile_toggle(GtkCellRendererToggle *cell_renderer, gchar *path, gpointer user_data);
void callback_settings_window_close(GtkButton *widget, gpointer user_data);
void callback_profile_selected(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data);
void callback_profile_name_changed(GtkEditable *editable, gpointer user_data);
void callback_profile_setting_checked(GtkToggleButton *togglebutton, gpointer user_data);
void callback_profile_create(GtkMenuItem *menuitem, gpointer user_data);
void callback_profile_delete(GtkMenuItem *menuitem, gpointer user_data);
gboolean callback_profile_context_menu(GtkWidget *tree_view, GdkEventButton *event, gpointer user_data);
void callback_page_previous(GtkButton *widget, gpointer user_data);
void callback_page_next(GtkButton *widget, gpointer user_data);
void callback_show_archives(GtkMenuItem *menuitem, gpointer user_data);
void callback_browsing_archives_select(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data);
gboolean callback_browsing_archives_context_menu(GtkWidget *tree_view, GdkEventButton *event, gpointer user_data);
void callback_browsing_archives_refresh (GtkMenuItem *menuitem, gpointer user_data);
void callback_browsing_archives_delete (GtkMenuItem *menuitem, gpointer user_data);
void callback_mail_archive(GtkMenuItem *menuitem, gpointer user_data);
void callback_show_archived_mail(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data);

#endif /* SRC_CALLBACKS_H_ */
