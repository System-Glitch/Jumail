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

#if defined(_WIN32)
	#define MAIL_FOLDER "Mail\\"
#else
	#define MAIL_FOLDER "Mail/"
#endif
#define MAIL_FILENAME_END ".txt"

void checkDirectoryExistArchive();
void createMailFile(Email *, char *);
void createFolderForMail(char *, char *);
Email * readEmailFile(char *);

#endif /* ARCHIVE_H_ */
