/*
 * config.h
 *
 *  Created on: 5 janv. 2018
 *      Author: louis
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include "library.h"
#include "utilities.h"
#include "LinkedList.h"
#include "profils.h"

#define PATH "config.xml"

void updateConfig(Profile *);
void checkDirectoryExistConfig();
char * parseFileConfig(xmlNode *, int *);
Profile * searchProfile();
char * loadConfig();

#endif /* CONFIG_H_ */
