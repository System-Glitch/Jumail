/*
 * profils.h
 *
 *  Created on: 4 déc. 2017
 *      Author: louis
 */

#ifndef PROFILS_H_
#define PROFILS_H_
#include "library.h"
#include "LinkedList.h"

#if defined(_WIN32)
	#define PROFILE_FILENAME_START "Profils\\"
#else
	#define PROFILE_FILENAME_START "Profils/"
#endif

#define PROFILE_FILENAME_END ".xml"
extern linkedlist_t * listProfile;

typedef struct Profile{
	char * sendP;
	char * SslSmtp;
	char * TlsSmtp;
	char * receiveP;
	char * SslImap;
	char * emailAddress;
	char * password;
	char * name;
	char * fullName;

}Profile;

void saveProfile(Profile *, char *);
Profile * loadProfile(char *, Profile *, char *);
void showProfile(Profile *);
Profile * initProfile();
void get_attribute(xmlNode *, char **);
Profile * parseFile(xmlNode *, Profile *);
void check(FILE *);
void freeProfile(Profile *);
void freeListProfile();
int loadAllProfile();
void checkDirectoryExist();

#endif /* PROFILS_H_ */
