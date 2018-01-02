/**
 * 	callbacks.h
 *
 *  Author : Jérémy LAMBERT
 *  Date : 02/01/2018
 *  Description : Holds all the callbacks for the GUI
 */

#ifndef SRC_CALLBACKS_H_
#define SRC_CALLBACKS_H_

typedef struct
{
	GtkBuilder *builder;
	gpointer user_data;
} SGlobalData;

/**
 * Displays a modal error dialog.
 */
void window_show_error(const char * message, SGlobalData *data);

void callback_quit(GtkMenuItem *menuitem, gpointer user_data);
void callback_about (GtkMenuItem *menuitem, gpointer user_data);
void callback_browsing_select(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data);

#endif /* SRC_CALLBACKS_H_ */
