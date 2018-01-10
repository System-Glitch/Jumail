/**
 * 	ArchivesWindow.h
 *
 *  Author : Jérémy LAMBERT
 *  Date : 05/01/2018
 *  Description : Creates and manage the archive window
 */

#ifndef SRC_GUI_ARCHIVESWINDOW_H_
#define SRC_GUI_ARCHIVESWINDOW_H_

#include <stdlib.h>
#include <gtk/gtk.h>
#include "callbacks.h"
#include "../LinkedList.h"
#include "../Archive.h"

extern linkedlist_t * loaded_archived_mails;

void free_list_loaded_archived_mails();

/**
 * Empty and load the tree store with the available folders.
 * Return 1 if success, 0 on failure. Triggers an error dialog.
 */
int tree_browsing_archives_refresh(SGlobalData *data);

/**
 * Gets all mails in the given folder and display on the GUI. Returns 1 on success, 0 on failure.
 */
int browsing_refresh_archives_folder(char * folder, SGlobalData *data);

/**
 * Gets the selected row in the browsing tree and puts the reference to the value of the row in "string"
 */
void tree_browsing_archives_get_selected_row(SGlobalData *data, gchar **string, GtkTreeIter *iter);

/**
 * Returns the index of the selected row in the folder content view. Returns -1 if nothing is selected
 */
int archive_list_folder_get_selected_row(SGlobalData *data, GtkTreeIter *iter);

#endif /* SRC_GUI_ARCHIVESWINDOW_H_ */
