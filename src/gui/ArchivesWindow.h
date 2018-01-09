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

#endif /* SRC_GUI_ARCHIVESWINDOW_H_ */
