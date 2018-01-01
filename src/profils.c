/*
 * profils.c
 *
 *  Created on: 4 déc. 2017
 *      Author: louis
 */
#include "profils.h"

linkedlist_t * listProfile = NULL;
// Fonction de création du fichier xml du profil
void createNewProfile(Profile * profile){
	xmlDocPtr doc = NULL;
	xmlNodePtr root_node = NULL, node = NULL;
	char * filename = NULL;
	filename = malloc(strlen(PROFILE_FILENAME_START) + strlen(PROFILE_FILENAME_END) + strlen(profile->nameOfProfile) + 1);
	if(filename == NULL){
		printf("Erreur allocation ! \n");
		exit(EXIT_FAILURE);
	}
	strcpy(filename, PROFILE_FILENAME_START);
	strcat(filename, profile->nameOfProfile);
	strcat(filename, PROFILE_FILENAME_END);
	// Données

	doc = xmlNewDoc(BAD_CAST "1.0");
	root_node = xmlNewNode(NULL, BAD_CAST "Profile");
	xmlDocSetRootElement(doc, root_node);

	// Les données
	node = xmlNewChild(root_node, NULL, BAD_CAST "Name", NULL);
	xmlNewProp(node, BAD_CAST "Value", BAD_CAST (profile)->userName);

	node = xmlNewChild(root_node, NULL, BAD_CAST "Password", NULL);
	xmlNewProp(node, BAD_CAST "Value", BAD_CAST (profile)->password);

	node = xmlNewChild(root_node, NULL, BAD_CAST "Send", NULL);
	xmlNewProp(node, BAD_CAST "Value", BAD_CAST (profile)->sendP);

	node = xmlNewChild(root_node, NULL, BAD_CAST "Receive", NULL);
	xmlNewProp(node, BAD_CAST "Value", BAD_CAST (profile)->receiveP);

	node = xmlNewChild(root_node, NULL, BAD_CAST "fullName", NULL);
	xmlNewProp(node, BAD_CAST "Value", BAD_CAST (profile)->fullName);


	// Création d'un fichier ou affichage dans la console
	xmlSaveFormatFileEnc(filename, doc, "UTF-8", 1);

	// Libération du document
	xmlFreeDoc(doc);

	free(filename);

	xmlCleanupParser();

	xmlMemoryDump();

}

// Fonction de debug pour affichage de la structure profil
void showProfile(Profile * profile){
	printf("%s\n", profile->receiveP == NULL ? "(null)" : profile->receiveP);
	printf("%s\n", profile->sendP == NULL ? "(null)" : profile->sendP);
	printf("%s\n", profile->userName == NULL ? "(null)" : profile->userName);
	printf("%s\n", profile->password == NULL ? "(null)" : profile->password);
	printf("%s\n", profile->nameOfProfile == NULL ? "(null)" : profile->nameOfProfile);
	printf("%s\n", profile->fullName == NULL ? "(null)" : profile->fullName);
}

// Verrification de l'ouverture du fichier
void check(FILE * fileToCheck){
	if(fileToCheck != NULL){
		printf("Problème avec le fichier");
		exit(EXIT_FAILURE);

	}

}

// Initialisation de la structure
Profile * initProfile(){
	Profile * profile = NULL;
	profile = malloc(sizeof(Profile) * 1);
	if(profile == NULL){
		printf("Erreur d'allocation ! \n");
		return NULL;
	}
	profile->nameOfProfile = NULL;
	profile->password = NULL;
	profile->receiveP = NULL;
	profile->sendP = NULL;
	profile->userName = NULL;
	profile->fullName = NULL;
	return profile;
}

// Chargement d'un fichier
Profile * loadProfile(char * fileName1, Profile * profile, char * fileName2){
	xmlDoc *doc = NULL;
	xmlNode *root_element = NULL;
	int cpt = 0;

	profile = initProfile();
	if(profile == NULL){
		return NULL;
	}

	doc = xmlReadFile(fileName1, NULL, 0);
	if (doc == NULL){
		printf("error: could not parse file %s\n", fileName1);
		free(profile);
		return NULL;

	}else{

		// Get the root element node
		root_element = xmlDocGetRootElement(doc);

		// Récupération des données
		profile = parseFile(root_element, &cpt, profile);
		profile->nameOfProfile = malloc(strlen(fileName2)+1);
		if(profile->userName  == NULL) {
			printf("Erreur allocation userName ! \n");
			freeProfile(profile);
			free(profile);
			xmlFreeDoc(doc);
			return NULL;
		}
		strcpy(profile->nameOfProfile, fileName2);
		profile->nameOfProfile[strlen(profile->nameOfProfile)-4] = '\0';

		// Libération du document
		xmlFreeDoc(doc);

		return profile;
	}

	// Libération de la mémoire des fonctions
	xmlCleanupParser();
}

// Fonction d'attribution des données du parsing
Profile * parseFile(xmlNode * a_node, int * cpt, Profile * profile){
	xmlNode *cur_node = NULL;
	// cur_node est égale au xmlNode envoie puis cur_node avance de node en node
	for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
		if (cur_node->type == XML_ELEMENT_NODE) {
			if(!strcmp((char*)cur_node->name, "Profile")) {
				cur_node = cur_node->children;
				continue;
			}

			if(!strcmp((char*)cur_node->name, "Name")) {
				get_attribute(cur_node, &(profile->userName));

			} else if(!strcmp((char*)cur_node->name, "Password")) {
				get_attribute(cur_node, &(profile->password));

			} else if(!strcmp((char*)cur_node->name, "Send")) {
				get_attribute(cur_node, &(profile->sendP));

			} else if(!strcmp((char*)cur_node->name, "Receive")) {
				get_attribute(cur_node, &(profile->receiveP));

			} else if(!strcmp((char*)cur_node->name, "fullName")) {
				get_attribute(cur_node, &(profile->fullName));
			}
		}
	}
	return profile;
}

// Fonction de libération la structure

void freeProfile(Profile * profile){
	if(profile->nameOfProfile != NULL)
		free(profile->nameOfProfile);
	if(profile->password != NULL)
		free(profile->password);
	if(profile->receiveP != NULL)
		free(profile->receiveP);
	if(profile->sendP != NULL)
		free(profile->sendP);
	if(profile->userName != NULL)
		free(profile->userName);
	if(profile->fullName != NULL)
		free(profile->fullName);

}

// #balanceTonAttribut
void get_attribute(xmlNode *node, char ** ptr) {
	xmlAttr* attribute = node->properties;
	while(attribute) {

		if(!strcmp((char*)attribute->name, "Value")) {
			xmlChar* value = xmlNodeListGetString(node->doc, attribute->children, 1);
			*ptr = malloc(strlen((char*)value)+1);
			if(*ptr == NULL) {
				fputs("Error while allocating for Profile attribute.\n", stderr);
				return;
			}
			strcpy(*ptr, (char*)value);
			xmlFree(value);
			return;
		}
		attribute = attribute->next;
	}
}

// Charger tous les profils
int loadAllProfile(){

	Profile * profile = NULL;
	DIR* rep = NULL; // Création varaible dossier
	struct dirent* file = NULL; /* Déclaration d'un pointeur vers la structure dirent. */
	char * filename = NULL;

	freeListProfile();
	listProfile = linkedlist_init();
	// Ouverture de dossier
	rep = opendir(PROFILE_FILENAME_START);

	// Vérification de l'ouverture
	if (rep == NULL){
		printf("Erreur à l'ouverture du dossier ! \n");
		return -2;
	}

	file = readdir(rep);
	file = readdir(rep);
	// Lecture des fichiers des du dossier
	while ((file = readdir(rep)) != NULL){
		filename = malloc(strlen(PROFILE_FILENAME_START) + strlen(file->d_name) + 1);
		if(filename == NULL){
			printf("Erreur allocation ! \n");
			return -1;
		}
		// Pour le nom de fichier
		strcpy(filename, PROFILE_FILENAME_START);
		strcat(filename, file->d_name);

		// Chargement de la structure
		profile = loadProfile(filename, profile, file->d_name);
		if(profile != NULL){
			// Ajout à la liste
			linkedlist_add(listProfile, profile);
		}
		free(filename);
	}

	// Fermeture du dossier
	if (closedir(rep) == -1){
		printf("erreur fermeture du dossier ! \n");
		return -3;
	}

	return 1;
}

// Libération de la liste
void freeListProfile(){
	if(listProfile == NULL)
		return;

	if(listProfile->length){
		node_t * current = listProfile->head;
		while(current != NULL){
			freeProfile(current->val);
			current = current->next;
		}
	}
	linkedlist_free(listProfile);
}

// Fonction de verification de l'existance du dossier
void checkDirectoryExist(){
	#if defined(_WIN32)
		DIR* dir = opendir(PROFILE_FILENAME_START);
	#else
		DIR* dir = opendir(PROFILE_FILENAME_START);
	#endif

	if (dir){
	    // Si oui
	    closedir(dir);

	}else if (ENOENT == errno){
	    // Sinon
		#if defined(_WIN32)
    _		mkdir(PROFILE_FIENAME_START);
     	 #else
    		mkdir(PROFILE_FILENAME_START, 0700);
     	 #endif

	}else{
	    // Erreur
		printf("Erreur à l'ouverture/creation du dossier Profils ! \n");
		exit(EXIT_FAILURE);

	}
}
