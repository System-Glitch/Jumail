/*
 * Archive.c
 *
 *  Created on: 1 janv. 2018
 *      Author: louis
 */
#include "Archive.h"

// Fonction de verification de l'existance du dossier
void checkDirectoryExistArchive(){
#if defined(_WIN32)
	DIR* dir = opendir(MAIL_FOLDER);
#else
	DIR* dir = opendir(MAIL_FOLDER);
#endif

	if (dir){
		// Si oui
		closedir(dir);

	}else if (ENOENT == errno){
		// Sinon
#if defined(_WIN32)
		_		mkdir(MAIL_FOLDER);
#else
		mkdir(MAIL_FOLDER, 0700);
#endif

	}else{
		// Erreur
		printf("Erreur à l'ouverture/creation du dossier Profils ! \n");
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
	printf("taille chaine : %ld\n", sizeof(filename2));
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
	printf("coucou 1 \n\n");
	printf("%s\n", email->raw);
	fwrite(email->raw, sizeof(char), strlen(email->raw),  file);
	printf("coucou 2 \n");

	fclose(file);
	free(filename2);
	free(filename);
}

// Création de dossier à partir d'un chemin relatif et d'un nom
void createFolderForMail(char * path, char * folderName){
	char * finalName = NULL;

	finalName = malloc(strlen(path) + strlen(folderName) + 1);

	if(folderName == NULL || finalName == NULL){
		printf("Erreur creation du dossier probleme alloction ou les entrees sont NULL ! \n");
		return;
	}

	strcpy(finalName, path);
	strcat(finalName, folderName);
#if defined(_WIN32)
	mkdir(finalName);
#else
	mkdir(finalName, 0700);
#endif

	free(finalName);
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

