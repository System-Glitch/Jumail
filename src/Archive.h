/*
 * Archive.h
 *
 *  Created on: 1 janv. 2018
 *      Author: louis
 */

#ifndef ARCHIVE_H_
#define ARCHIVE_H_

#include "library.h"
#include "mailing.h"
#include "utilities.h"
#include "LinkedList.h"

#if defined(_WIN32)
	#define ARCHIVES_FOLDER_NAME "Archives\\"
#else
	#define ARCHIVES_FOLDER_NAME "Archives/"
#endif
#define MAIL_FILENAME_END ".txt"

void checkDirectoryExistArchive();
void createMailFile(Email *, char *);
int createFolderForMail(char **);
Email * readEmailFile(char *);

/**
 * Lists recursively all the folders in the archives folder and fills the given linked list with the result.
 */
void list_archives_folders(linkedlist_t *list, char *path);

int remove_archives_dir(char* dirname);

int archives_load_folder(linkedlist_t *list, linkedlist_t *list_paths, char *path);

int move_archived_mail(char *path, char *dst_path);

#endif /* ARCHIVE_H_ */
