/**
 * 	SettingsWindow.c
 *
 *  Author : Jérémy LAMBERT
 *  Date : 05/01/2018
 *  Description : Creates and manage the settings window
 */
#ifndef SETTINGSWINDOW_H_
#define SETTINGSWINDOW_H_

#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include "callbacks.h"

void init_settings_window(SGlobalData *data);
void open_settings_window(SGlobalData *data);

#endif
