/*
 * profils.c
 *
 *  Created on: 4 déc. 2017
 *      Author: louis
 */
#include "profils.h"
#include "libs/b64/b64.h"

linkedlist_t * listProfile = NULL;
Profile *current_profile = NULL;

static char cipher_key[21] = "Wqzt7iRS7D5HOGy08Luq";

static void xor_cipher(char *str, size_t len) {
	for(size_t i = 0 ; i < len ; i++) {
		str[i] ^= cipher_key[i%20] + 1;
	}
}

static char* cipher_password(char *password) {
	char *cpy = NULL;
	char *result = NULL;
	size_t len;

	if(password == NULL) return NULL;

	len = strlen(password);
	cpy = malloc(len+1);
	if(cpy == NULL) return NULL;

	strcpy(cpy,password);

	xor_cipher(cpy,len);

	//base64 to make it parsable by libxml
	result = b64_encode((unsigned char*)cpy, len);
	return result;
}

static char* decipher_password(char *password) {
	char *cpy = NULL;
	int len;

	if(password == NULL) return NULL;

	len = strlen(password);
	cpy = (char*)b64_decode(password, len);

	xor_cipher(cpy,strlen(cpy));

	return cpy;
}


// Fonction de création du fichier xml du profil
void saveProfile(Profile * profile, char * previous_name){
	xmlDocPtr doc = NULL;
	xmlNodePtr root_node = NULL, node = NULL;
	char * filename = NULL;
	char * filename2 = NULL;
	char *cpy = NULL;
	filename = malloc(strlen(PROFILE_FILENAME_START) + strlen(PROFILE_FILENAME_END) + strlen(profile->name) + 1);
	if(previous_name != NULL){
		filename2 = malloc(strlen(PROFILE_FILENAME_START) + strlen(PROFILE_FILENAME_END) + strlen(previous_name) + 1);
		if(filename2 == NULL){
			printf("Erreur allocation ! \n");
			free(filename);
			return;
		}
		strcpy(filename2, PROFILE_FILENAME_START);
		strcat(filename2, previous_name);
		strcat(filename2, PROFILE_FILENAME_END);
	}
	if(filename == NULL){
		printf("Erreur allocation ! \n");
		exit(EXIT_FAILURE);
	}
	if(previous_name != NULL && strcmp(profile->name, previous_name) != 0){
		remove(filename2);
	}
	strcpy(filename, PROFILE_FILENAME_START);
	strcat(filename, profile->name);
	strcat(filename, PROFILE_FILENAME_END);
	// Données

	doc = xmlNewDoc(BAD_CAST "1.0");
	root_node = xmlNewNode(NULL, BAD_CAST "Profile");
	xmlDocSetRootElement(doc, root_node);

	// Les données
	node = xmlNewChild(root_node, NULL, BAD_CAST "Name", NULL);
	xmlNewProp(node, BAD_CAST "Value", BAD_CAST profile->emailAddress);
	node = xmlNewChild(root_node, NULL, BAD_CAST "Password", NULL);
	cpy = cipher_password(profile->password);

	xmlNewProp(node, BAD_CAST "Value", BAD_CAST cpy);

	node = xmlNewChild(root_node, NULL, BAD_CAST "Send", NULL);
	xmlNewProp(node, BAD_CAST "Value", BAD_CAST profile->sendP);

	node = xmlNewChild(root_node, NULL, BAD_CAST "Receive", NULL);
	xmlNewProp(node, BAD_CAST "Value", BAD_CAST profile->receiveP);

	node = xmlNewChild(root_node, NULL, BAD_CAST "fullName", NULL);
	xmlNewProp(node, BAD_CAST "Value", BAD_CAST profile->fullName);

	node = xmlNewChild(root_node, NULL, BAD_CAST "SSL_IMAP", NULL);
	xmlNewProp(node, BAD_CAST "Value", BAD_CAST profile->SslImap);

	node = xmlNewChild(root_node, NULL, BAD_CAST "SSL_SMTP", NULL);
	xmlNewProp(node, BAD_CAST "Value", BAD_CAST profile->SslSmtp);

	node = xmlNewChild(root_node, NULL, BAD_CAST "TLS_SMTP", NULL);
	xmlNewProp(node, BAD_CAST "Value", BAD_CAST profile->TlsSmtp);




	// Création d'un fichier ou affichage dans la console
	xmlSaveFormatFileEnc(filename, doc, "UTF-8", 1);

	// Libération du document
	xmlFreeDoc(doc);

	free(filename);
	if(cpy != NULL)
		free(cpy);

	xmlCleanupParser();

	xmlMemoryDump();

}

// Fonction de debug pour affichage de la structure profil
void showProfile(Profile * profile){
	printf("%s\n", profile->receiveP == NULL ? "(null)" : profile->receiveP);
	printf("%s\n", profile->sendP == NULL ? "(null)" : profile->sendP);
	printf("%s\n", profile->emailAddress == NULL ? "(null)" : profile->emailAddress);
	printf("%s\n", profile->password == NULL ? "(null)" : profile->password);
	printf("%s\n", profile->name == NULL ? "(null)" : profile->name);
	printf("%s\n", profile->fullName == NULL ? "(null)" : profile->fullName);
	printf("%s\n", profile->SslImap == NULL ? "(null)" : profile->SslImap);
	printf("%s\n", profile->SslSmtp == NULL ? "(null)" : profile->SslSmtp);
	printf("%s\n", profile->TlsSmtp == NULL ? "(null)" : profile->TlsSmtp);
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
	profile->name = NULL;
	profile->password = NULL;
	profile->receiveP = NULL;
	profile->sendP = NULL;
	profile->emailAddress = NULL;
	profile->fullName = NULL;
	profile->SslImap = NULL;
	profile->SslSmtp = NULL;
	profile->TlsSmtp = NULL;
	return profile;
}

// Chargement d'un fichier
Profile * loadProfile(char * fileName1, Profile * profile, char * fileName2){
	xmlDoc *doc = NULL;
	xmlNode *root_element = NULL;

	profile = initProfile();
	if(profile == NULL){
		return NULL;
	}

	doc = xmlReadFile(fileName1, NULL, 0);
	if (doc == NULL){
		printf("Invalid profile file %s. Skipping.\n", fileName1);
		free(profile);
		return NULL;

	}else{

		// Get the root element node
		root_element = xmlDocGetRootElement(doc);

		// Récupération des données
		profile = parseFile(root_element, profile);
		profile->name = malloc(strlen(fileName2)+1);
		if(profile->emailAddress  == NULL) {
			printf("Erreur allocation userName ! \n");
			freeProfile(profile);
			free(profile);
			xmlFreeDoc(doc);
			return NULL;
		}
		strcpy(profile->name, fileName2);
		profile->name[strlen(profile->name)-4] = '\0';

		// Libération du document
		xmlFreeDoc(doc);

		return profile;
	}

	// Libération de la mémoire des fonctions
	xmlCleanupParser();
}

// Fonction d'attribution des données du parsing
Profile * parseFile(xmlNode * a_node, Profile * profile){
	xmlNode *cur_node = NULL;
	char *password;
	// cur_node est égale au xmlNode envoie puis cur_node avance de node en node
	for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
		if (cur_node->type == XML_ELEMENT_NODE) {
			if(!strcmp((char*)cur_node->name, "Profile")) {
				cur_node = cur_node->children;
				continue;
			}

			if(!strcmp((char*)cur_node->name, "Name")) {
				get_attribute(cur_node, &(profile->emailAddress));

			} else if(!strcmp((char*)cur_node->name, "Password")) {
				get_attribute(cur_node, &(profile->password));
				password = decipher_password(profile->password);
				free(profile->password);
				profile->password = (char*)password;
			} else if(!strcmp((char*)cur_node->name, "Send")) {
				get_attribute(cur_node, &(profile->sendP));

			} else if(!strcmp((char*)cur_node->name, "Receive")) {
				get_attribute(cur_node, &(profile->receiveP));

			} else if(!strcmp((char*)cur_node->name, "fullName")) {
				get_attribute(cur_node, &(profile->fullName));

			} else if(!strcmp((char*)cur_node->name, "SSL_IMAP")){
				get_attribute(cur_node, &(profile->SslImap));

			} else if(!strcmp((char*)cur_node->name, "SSL_SMTP")){
				get_attribute(cur_node, &(profile->SslSmtp));

			} else if(!strcmp((char*)cur_node->name, "TLS_SMTP")){
				get_attribute(cur_node, &(profile->TlsSmtp));

			}
		}
	}
	return profile;
}

// Fonction de libération la structure

void freeProfile(Profile * profile){
	if(profile->name != NULL)
		free(profile->name);
	if(profile->password != NULL)
		free(profile->password);
	if(profile->receiveP != NULL)
		free(profile->receiveP);
	if(profile->sendP != NULL)
		free(profile->sendP);
	if(profile->emailAddress != NULL)
		free(profile->emailAddress);
	if(profile->fullName != NULL)
		free(profile->fullName);
	if(profile->SslImap != NULL)
		free(profile->SslImap);
	if(profile->SslSmtp != NULL)
		free(profile->SslSmtp);
	if(profile->TlsSmtp != NULL)
		free(profile->TlsSmtp);
}

// Récupération de la valeur d'un attribut
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
int loadAllProfile(char *selected_profile){

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

	// Lecture des fichiers des du dossier
	while ((file = readdir(rep)) != NULL){
		if(!strcmp(file->d_name, ".") || !strcmp(file->d_name, "..")) continue;
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

			if(strcmp(selected_profile,"$NULL") != 0 && !strcmp(profile->name, selected_profile))
				current_profile = profile;
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
void checkProfileDirectoryExist(){
	DIR* dir = opendir(PROFILE_FILENAME_START);

	if (dir){
		// Si oui
		closedir(dir);

	}else if (ENOENT == errno){
		// Sinon
#if defined(_WIN32)
		_mkdir(PROFILE_FILENAME_START);
#else
		mkdir(PROFILE_FILENAME_START, 0700);
#endif

	}else{
		// Erreur
		printf("Erreur à l'ouverture/creation du dossier Profils ! \n");
		exit(EXIT_FAILURE);

	}
}
