

/*
 * config.c
 *
 *  Created on: 5 janv. 2018
 *      Author: louis
 */
#include "config.h"

void updateConfig(Profile * profile){
	xmlDocPtr doc = NULL;
	xmlNodePtr root_node = NULL, node = NULL;

	// Données
	doc = xmlNewDoc(BAD_CAST "1.0");
	root_node = xmlNewNode(NULL, BAD_CAST "Config");
	xmlDocSetRootElement(doc, root_node);

	// Les données
	node = xmlNewChild(root_node, NULL, BAD_CAST "Name", NULL);
	xmlNewProp(node, BAD_CAST "Value", BAD_CAST (profile)->name);

	// Création d'un fichier ou affichage dans la console
	xmlSaveFormatFileEnc("config.xml", doc, "UTF-8", 1);

	// Libération du document
	xmlFreeDoc(doc);

	xmlCleanupParser();

	xmlMemoryDump();
}

Profile * searchProfile(){
	char * filename = NULL;
	Profile * profile = NULL;
	profile = initProfile();
	filename = loadConfig();
	if(filename == NULL||profile == NULL){
		printf("Erreur retour loadConfig ou allocation profile ! \n");
		free(filename);
		return NULL;
	}
	if(strcmp(filename, "$NULL") == 0){
		printf("Aucun fichier changer ! \n");
		free(filename);
		return NULL;
	}
	if(listProfile->length){
		node_t * current = listProfile->head;
		while(current != NULL){
			profile = (Profile *)current->val;
			if(strcmp(filename, profile->name) == 0){
				free(filename);
				return profile;
			}
			current = current->next;
		}
	}
	free(filename);
	printf("Aucun fichier ne correspond ! \n");
	return profile;
}

char * loadConfig(){
	xmlDoc *doc = NULL;
	xmlNode *root_element = NULL;
	int cpt = 0;
	char * filename = NULL;

	if( access( PATH, F_OK ) != -1 ) {
		doc = xmlReadFile(PATH, NULL, 0);
		if (doc == NULL){
			printf("error: could not parse file %s\n", PATH);
			return NULL;

		}else{

			// Get the root element node
			root_element = xmlDocGetRootElement(doc);

			// Récupération des données
			filename = parseFileConfig(root_element, &cpt);
			if(filename  == NULL) {
				printf("Erreur retour filename ! \n");
				xmlFreeDoc(doc);
				return NULL;
			}

			// Libération du document
			xmlFreeDoc(doc);
			return filename;
		}

		// Libération de la mémoire des fonctions
		xmlCleanupParser();
	} else {
		fputs("Config file not found. Creating a new one\n", stdout);
		Profile *profile = initProfile();
		if(profile == NULL) return NULL;
		profile->name = "$NULL";
		updateConfig(profile);

		free(profile);

		filename = malloc(6);
		if(filename == NULL) {
			fputs("Not enough memory.\n", stderr);
			return NULL;
		}
		strcpy(filename, "$NULL");
		return filename;
	}

}

char * parseFileConfig(xmlNode * a_node, int * cpt){
	xmlNode *cur_node = NULL;
	Profile * profile = NULL;
	char * filename = NULL;

	profile = initProfile();
	if(profile == NULL){
		printf("Erreur allocation profile ! \n");
		return NULL;
	}
	filename = malloc(sizeof(char) * 250);
	if(filename == NULL){
		printf("Erreur allocatuon filename ! \n");
		free(profile);
		return NULL;
	}

	// cur_node est égale au xmlNode envoie puis cur_node avance de node en node
	for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
		if (cur_node->type == XML_ELEMENT_NODE) {
			if(!strcmp((char*)cur_node->name, "Config")) {
				cur_node = cur_node->children;
				continue;
			}

			if(!strcmp((char*)cur_node->name, "Name")) {
				get_attribute(cur_node, &profile->name);
				strcpy(filename, profile->name);
				free(profile);
				return filename;
			}
		}
	}
	return NULL;
}


