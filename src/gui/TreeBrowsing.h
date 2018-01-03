/**
 * 	TreeBrowsing.h
 *
 *  Author : Jérémy LAMBERT
 *  Date : 02/01/2018
 *  Description : Handler for the folder navigation menu
 */
#ifndef SRC_GUI_TREEBROWSING_H_
#define SRC_GUI_TREEBROWSING_H_

#include "../folders.h"

/**
 * Gets the selected row in the browsing tree and puts the reference to the value of the row in "string"
 */
void tree_browsing_get_selected_row(SGlobalData *data, gchar **string, GtkTreeIter *iter);

/**
 * Empty and load the tree store with the available folders. Safely checks if a profile is selected or not.
 * Return 1 if success, 0 on failure. Triggers an error dialog.
 */
int tree_browsing_refresh(SGlobalData *data);

/**
 * Gets all mails in the given folder and display on the GUI. Returns 1 on success, 0 on failure.
 */
int browsing_refresh_folder(char * folder, SGlobalData *data);

/**
 * Returns the index of the selected row in the folder content view. Returns -1 if nothing is selected
 */
int list_folder_get_selected_row(SGlobalData *data, GtkTreeIter *iter);

#endif /* SRC_GUI_TREEBROWSING_H_ */
