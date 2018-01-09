/*
 * Archive.c
 *
 *  Created on: 1 janv. 2018
 *      Author: louis
 */
#include "Archive.h"

// Fonction de verification de l'existance du dossier
void checkDirectoryExistArchive(){
	DIR* dir = opendir(ARCHIVES_FOLDER_NAME);

	if (dir){
		// Si oui
		closedir(dir);

	}else if (ENOENT == errno){
		// Sinon
#if defined(_WIN32)
		_mkdir(ARCHIVES_FOLDER_NAME);
#else
		mkdir(ARCHIVES_FOLDER_NAME, 0700);
#endif

	}else{
		// Erreur
		printf("Erreur à l'ouverture/creation du dossier Archives ! \n");
		exit(EXIT_FAILURE);

	}
}

// Creation d'un fichier mail
void createMailFile(Email * email, char * path){
	FILE * file = NULL;
	char * filename = NULL;
	char * filename2 = NULL;

	// Génération d'un id unique
	filename = generate_id();
	printf("id : %s\n", filename);
	filename2 = malloc(strlen(filename) + strlen(path) + strlen(MAIL_FILENAME_END) + 1);
	if(filename == NULL||filename2 == NULL){
		printf("Erreur generation email id ou allocation ! \n");
		fclose(file);
		return;
	}

	// Création du nom
	strcpy(filename2, path);
	strcat(filename2, filename);
	strcat(filename2, MAIL_FILENAME_END);

	printf("path : %s\n", filename2);
	file = fopen(filename2, "w");
	if(file == NULL){
		printf("Erreur création du fichier ! \n");
		free(filename2);
		free(filename);
		return;

	}
	fwrite(email->raw, sizeof(char), strlen(email->raw),  file);

	fclose(file);
	free(filename2);
	free(filename);
}

// Création de dossier à partir d'un chemin relatif et d'un nom
int createFolderForMail(char ** path){
	char * finalName = NULL;
	int cmp = 0;
	int status = 0;

	if(path == NULL || *path == NULL){
		printf("Erreur creation du dossier les entrees sont NULL ! \n");
		return -1;
	}

	cmp = strncmp(*path,ARCHIVES_FOLDER_NAME,9);
	if(cmp)
		finalName = malloc(9 + strlen(*path) + 1);
	else
		finalName = malloc(strlen(*path) + 1);

	if(finalName == NULL) {
		printf("Erreur creation du dossier allocation ! \n");
		return -1;
	}

	if(cmp != 0) {
		strcpy(finalName, ARCHIVES_FOLDER_NAME);
		strcat(finalName, *path);
	} else
		strcpy(finalName, *path);

#if defined(_WIN32)
	status = mkdir(finalName);
#else
	status = mkdir(finalName, 0700);
#endif

	*path = realloc(*path, strlen(finalName)+1);
	if(*path == NULL) {
		printf("Erreur creation du dossier allocation ! \n");
		return -1;
	}
	strcpy(*path,finalName);

	free(finalName);
	return status;
}

// Lecture d'un fichier
Email * readEmailFile(char * path){
	Email * email = NULL;
	FILE * file = NULL;
	char * fileContent = NULL;
	int size = 0;

	printf("path : %s\n", path);
	file = fopen(path, "r");
	if(file == NULL || path == NULL){
		printf("Erreur ouverture du fichier ou chemin egal null ! \n");
		return NULL;
	}
	fseek(file, 1, SEEK_SET);
	fseek(file, 1, SEEK_END);
	size = (int)ftell(file);
	printf("seek end : %d\n", size);

	fileContent = malloc(sizeof(char) * size);
	if(fileContent == NULL){
		printf("Erreur allocation ! \n");
		fclose(file);
		return NULL;
	}

	fseek(file, 1, SEEK_SET);
	fread(fileContent, sizeof(char), size, file);

	email = parse_email(fileContent);

	fclose(file);
	free(fileContent);
	return email;
}

/**
 * Lists recursively all the folders in the archives folder and fills the given linked list with the result.
 */
void list_archives_folders(linkedlist_t *list, char *path) {
	struct dirent *direntp = NULL;
	DIR *dirp = NULL;
	size_t path_len;
	char *full_name_cpy = NULL;

	/* Check input parameters. */
	if (!path)
		return;
	path_len = strlen(path);

	if (!path || !path_len || (path_len > 256))
		return;

	/* Open directory */
	dirp = opendir(path);
	if (dirp == NULL)
		return;

	while ((direntp = readdir(dirp)) != NULL)
	{
		/* For every directory entry... */
		struct stat fstat;
		char full_name[256 + 1];

		/* Calculate full name, check we are in file length limts */
		if ((path_len + strlen(direntp->d_name) + 1) > 256)
			continue;

		strcpy(full_name, path);
		if (full_name[path_len - 1] != FILE_SEPARATOR)
			strcat(full_name, FILE_SEPARATOR_STR);
		strcat(full_name, direntp->d_name);

		/* Ignore special directories. */
		if ((strcmp(direntp->d_name, ".") == 0) ||
				(strcmp(direntp->d_name, "..") == 0))
			continue;

		/* Print only if it is really directory. */
		if (stat(full_name, &fstat) < 0)
			continue;
		if (S_ISDIR(fstat.st_mode))
		{
			full_name_cpy = malloc(strlen(full_name)+1);
			if(full_name_cpy == NULL) {
				linkedlist_free(list);
				return;
			}
			strcpy(full_name_cpy, full_name);
			linkedlist_add(list, full_name_cpy);
			list_archives_folders(list, full_name);
		}
	}

	/* Finalize resources. */
	(void)closedir(dirp);
}

int remove_archives_dir(char* path) {
	struct dirent *direntp = NULL;
	DIR *dirp = NULL;
	size_t path_len;

	/* Check input parameters. */
	if (!path)
		return 0;
	path_len = strlen(path);

	if (!path || !path_len || (path_len > 256))
		return 0;

	/* Open directory */
	dirp = opendir(path);
	if (dirp == NULL)
		return 0;

	while ((direntp = readdir(dirp)) != NULL)
	{
		/* For every directory entry... */
		struct stat fstat;
		char full_name[256 + 1];

		/* Calculate full name, check we are in file length limts */
		if ((path_len + strlen(direntp->d_name) + 1) > 256)
			continue;

		strcpy(full_name, path);
		if (full_name[path_len - 1] != FILE_SEPARATOR)
			strcat(full_name, FILE_SEPARATOR_STR);
		strcat(full_name, direntp->d_name);

		/* Ignore special directories. */
		if ((strcmp(direntp->d_name, ".") == 0) ||
				(strcmp(direntp->d_name, "..") == 0))
			continue;

		/* Print only if it is really directory. */
		if (stat(full_name, &fstat) < 0)
			continue;
		if (S_ISDIR(fstat.st_mode))
		{
			remove_archives_dir(full_name);
		} else {
			remove(full_name);
		}
	}

	/* Finalize resources. */
	(void)closedir(dirp);
	rmdir(path);
	return 1;
}

