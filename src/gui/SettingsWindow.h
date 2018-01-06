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

/**
 * Gets the selected row in the profile tree and puts the reference to the value of the row in "string"
 */
void tree_profile_get_selected_row(SGlobalData *data, gchar **string, GtkTreeIter *iter);

void settings_window_set_all_fields_active(SGlobalData *data, gboolean active);

void settings_window_fill_entries(SGlobalData *data, Profile *profile);

#endif
