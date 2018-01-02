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
 * Empty and load the tree store with the available folders. Safely checks if a profile is selected or not.
 * Return 1 if success, 0 on failure. Triggers an error dialog.
 */
int tree_browsing_refresh(SGlobalData *data);

/**
 * Gets all mails in the given folder and display on the GUI. Returns 1 on success, 0 on failure.
 */
int browsing_refresh_folder(char * folder, SGlobalData *data);

#endif /* SRC_GUI_TREEBROWSING_H_ */
