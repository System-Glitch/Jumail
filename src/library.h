#ifndef LIBRARY_H_
#define LIBRARY_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Resources pour libxml2
#include <libxml/parser.h>
#include <libxml/tree.h>

// Resource pour manipulation des dossiers et des fichiers
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#if defined(_WIN32)
#include <direct.h>
#endif

#if defined(_WIN32)
	#define FILE_SEPARATOR '\\'
	#define FILE_SEPARATOR_STR "\\"
#else
	#define FILE_SEPARATOR '/'
	#define FILE_SEPARATOR_STR "/"
#endif

#endif /* LIBRARY_H_ */
