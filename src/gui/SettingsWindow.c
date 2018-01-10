/**
 * 	SettingsWindow.c
 *
 *  Author : Jérémy LAMBERT
 *  Date : 05/01/2018
 *  Description : Creates and manage the settings window
 */
#include "TreeBrowsing.h"
#include "SettingsWindow.h"
#include "../profils.h"
#include "../config.h"

static gboolean clear_profile_selection_tree_view(GtkTreeModel *model, GtkTreePath  *path, GtkTreeIter *iter, gpointer user_data) {
	gtk_list_store_set(GTK_LIST_STORE(model), iter, 0, FALSE, -1);
	return FALSE;
}

static void settings_window_set_field_active(SGlobalData *data, const char * name, gboolean active) {
	GtkWidget *widget = GTK_WIDGET (gtk_builder_get_object (data->builder, name));
	gtk_widget_set_sensitive(widget, active);
}

void settings_window_set_all_fields_active(SGlobalData *data, gboolean active) {
	settings_window_set_field_active(data, "ProfileEntryName", active);
	settings_window_set_field_active(data, "ProfileEntryAddress", active);
	settings_window_set_field_active(data, "ProfileEntryPassword", active);
	settings_window_set_field_active(data, "ProfileEntryFullName", active);
	settings_window_set_field_active(data, "ProfileEntryReceive", active);
	settings_window_set_field_active(data, "ProfileCheckSSLReceive", active);
	settings_window_set_field_active(data, "ProfileEntrySend", active);
	settings_window_set_field_active(data, "ProfileCheckSSLSend", active);
	settings_window_set_field_active(data, "ProfileCheckTLSSend", active);
}

static void settings_window_fill_entry(SGlobalData *data, const char *entry_name, char *text) {
	GtkEntry *widget = GTK_ENTRY (gtk_builder_get_object (data->builder, entry_name));
	gtk_entry_set_text(widget, text == NULL ? "" : text);
}

void settings_window_fill_entries(SGlobalData *data, Profile *profile) {
	GtkCheckButton *check;

	settings_window_fill_entry(data, "ProfileEntryName", profile == NULL ? NULL : profile->name);
	settings_window_fill_entry(data, "ProfileEntryAddress", profile == NULL ? NULL : profile->emailAddress);
	settings_window_fill_entry(data, "ProfileEntryPassword", profile == NULL ? NULL : profile->password);
	settings_window_fill_entry(data, "ProfileEntryFullName", profile == NULL ? NULL : profile->fullName);
	settings_window_fill_entry(data, "ProfileEntryReceive", profile == NULL ? NULL : profile->receiveP);
	settings_window_fill_entry(data, "ProfileEntrySend", profile == NULL ? NULL : profile->sendP);

	check = GTK_CHECK_BUTTON (gtk_builder_get_object (data->builder, "ProfileCheckSSLReceive"));
	if(profile == NULL)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), FALSE);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), profile->SslImap == NULL ? 0 : !strcmp(profile->SslImap, "TRUE"));

	check = GTK_CHECK_BUTTON (gtk_builder_get_object (data->builder, "ProfileCheckSSLSend"));
	if(profile == NULL)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), FALSE);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), profile->SslSmtp == NULL ? 0 : !strcmp(profile->SslSmtp, "TRUE"));

	check = GTK_CHECK_BUTTON (gtk_builder_get_object (data->builder, "ProfileCheckTLSSend"));
	if(profile == NULL)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), FALSE);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), profile->TlsSmtp == NULL ? 0 : !strcmp(profile->TlsSmtp, "TRUE"));

}

static gboolean check_if_name_exists(const char* name) {
	Profile *profile;
	if(listProfile->length){
		node_t * current = listProfile->head;
		while(current != NULL){
			profile = (Profile*)current->val;
			if(!strcmp(profile->name, name)) return TRUE;
			current = current->next;
			profile = (Profile*)current->val;
		}
	}
	return FALSE;
}

static void edit_string(char **str, const char *new_str, SGlobalData *data) {
	if(str == NULL || new_str == NULL) return;
	*str = realloc(*str, strlen(new_str)+1);
	if(*str == NULL) {
		window_show_error("Une erreur est survenue.\nMémoire insuffisante.", data, "SettingsWindow");
		return;
	}
	strcpy(*str, new_str);
}

gboolean callback_settings_entry_lose_focus(GtkWidget *widget, GdkEvent *event, gpointer user_data) {
	SGlobalData *data = (SGlobalData*) user_data;
	Profile *profile;
	const char *string;

	profile = (Profile*)linkedlist_get(listProfile, data->selected_profile_index);
	string = gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(widget)));

	if(widget == GTK_WIDGET(gtk_builder_get_object (data->builder, "ProfileEntryAddress")))
		edit_string(&profile->emailAddress, string, data);
	else if(widget == GTK_WIDGET(gtk_builder_get_object (data->builder, "ProfileEntryPassword")))
		edit_string(&profile->password, string, data);
	else if(widget == GTK_WIDGET(gtk_builder_get_object (data->builder, "ProfileEntryFullName")))
		edit_string(&profile->fullName, string, data);
	else if(widget == GTK_WIDGET(gtk_builder_get_object (data->builder, "ProfileEntryReceive")))
		edit_string(&profile->receiveP, string, data);
	else if(widget == GTK_WIDGET(gtk_builder_get_object (data->builder, "ProfileEntrySend")))
		edit_string(&profile->sendP, string, data);

	saveProfile(profile,profile->name);
	return FALSE;
}

void callback_profile_setting_checked(GtkToggleButton *togglebutton, gpointer user_data) {
	SGlobalData *data = (SGlobalData*) user_data;
	Profile *profile;

	profile = (Profile*)linkedlist_get(listProfile, data->selected_profile_index);

	if(profile != NULL) {
		if(togglebutton == GTK_TOGGLE_BUTTON(gtk_builder_get_object (data->builder, "ProfileCheckSSLReceive")))
			edit_string(&profile->SslImap, gtk_toggle_button_get_active(togglebutton) ? "TRUE" : "FALSE", data);
		else if(togglebutton == GTK_TOGGLE_BUTTON(gtk_builder_get_object (data->builder, "ProfileCheckSSLSend")))
			edit_string(&profile->SslSmtp, gtk_toggle_button_get_active(togglebutton) ? "TRUE" : "FALSE", data);
		else if(togglebutton == GTK_TOGGLE_BUTTON(gtk_builder_get_object (data->builder, "ProfileCheckTLSSend")))
			edit_string(&profile->TlsSmtp, gtk_toggle_button_get_active(togglebutton) ? "TRUE" : "FALSE", data);

		saveProfile(profile,profile->name);
	}

}

//Update the profile name in the list
void callback_profile_name_changed(GtkEditable *editable, gpointer user_data) { //TODO call on focus out
	SGlobalData *data = (SGlobalData*) user_data;
	GtkTreeModel *model;
	GtkWidget *tree_view;
	GtkTreeIter iter;
	GtkEntry *entry;
	GtkTreeSelection *selection;
	Profile *profile;
	const char *string;
	char *previous_name;

	tree_view = GTK_WIDGET(gtk_builder_get_object (data->builder, "TreeViewProfiles"));
	model = gtk_tree_view_get_model (GTK_TREE_VIEW(tree_view));
	entry = GTK_ENTRY (gtk_builder_get_object (data->builder, "ProfileEntryName"));
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));
	string = gtk_entry_buffer_get_text(gtk_entry_get_buffer(entry));
	gtk_tree_selection_get_selected (selection, &model, &iter);

	if (strlen(string) > 0 && !check_if_name_exists(string)) {

		profile = (Profile*)linkedlist_get(listProfile, data->selected_profile_index);

		previous_name = malloc(strlen(profile->name)+1);
		profile->name = realloc(profile->name, strlen(string)+1);
		if(profile->name == NULL || previous_name == NULL) {
			window_show_error("Une erreur est survenue.\nMémoire insuffisante.", data, "SettingsWindow");
			return;
		}

		strcpy(previous_name, profile->name);
		strcpy(profile->name, string);
		saveProfile(profile,previous_name);
		free(previous_name);

		if(profile == current_profile)
			updateConfig(profile);

		gtk_list_store_set (GTK_LIST_STORE(model), &iter, 1, string, -1);

	}
}

void callback_profile_selected(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data) {
	gint *i;
	Profile *profile;
	SGlobalData *data = (SGlobalData*) user_data;

	i = gtk_tree_path_get_indices (path);
	profile = (Profile*)linkedlist_get(listProfile, *i);
	if(profile == NULL) {
		window_show_error("Une erreur est survenue.\nLe profil sélectionné n'existe pas.", data, "SettingsWindow");
	} else {
		data->selected_profile_index = *i;
		settings_window_fill_entries(data, profile);
		settings_window_set_all_fields_active(data, TRUE);
	}

}

void callback_profile_toggle(GtkCellRendererToggle *cell_renderer, gchar *path, gpointer user_data) {
	SGlobalData *data = (SGlobalData*) user_data;
	GtkListStore *model;
	GtkTreeView *tree_view;
	GtkTreePath *tree_path;
	gint *i;
	GtkTreeIter iter;
	Profile *profile;

	tree_view = GTK_TREE_VIEW (gtk_builder_get_object (data->builder, "TreeViewProfiles"));
	model = GTK_LIST_STORE(gtk_tree_view_get_model(tree_view));
	tree_path = gtk_tree_path_new_from_string(path);
	i = gtk_tree_path_get_indices (tree_path);

	profile = linkedlist_get(listProfile, *i);
	if(profile == NULL) {
		window_show_error("Une erreur est survenue.\nProfil sélectionné introuvable.", data, "SettingsWindow");
		return;
	}

	//Clear all selection
	gtk_tree_model_foreach (GTK_TREE_MODEL(model), clear_profile_selection_tree_view, NULL);

	//Set the selected one to "activated"
	gtk_tree_model_get_iter(GTK_TREE_MODEL(model), &iter, tree_path);
	gtk_list_store_set(model, &iter, 0, TRUE, -1);

	settings_window_set_all_fields_active(data, TRUE);

	//Change active profile
	current_profile = profile;
	updateConfig(profile);

}

void init_settings_window(SGlobalData *data) {
	GtkTreeIter iter;
	GtkListStore *model;
	GtkTreeView *tree_view;
	GtkTreeViewColumn *column;
	GtkCellRenderer *renderer;
	GtkCellRenderer *renderer_toggle;

	fputs("Initializing settings window...\n", stdout);

	tree_view = GTK_TREE_VIEW (gtk_builder_get_object (data->builder, "TreeViewProfiles"));
	if(tree_view == NULL) {
		window_show_error("Impossible de charger l'interface de préférences.", data, "MainWindow");
		return;
	}

	model = gtk_list_store_new (2, G_TYPE_BOOLEAN, G_TYPE_STRING);
	gtk_tree_view_set_model (tree_view, GTK_TREE_MODEL (model));

	renderer_toggle = gtk_cell_renderer_toggle_new ();
	gtk_cell_renderer_toggle_set_radio(GTK_CELL_RENDERER_TOGGLE(renderer_toggle), TRUE);
	column = gtk_tree_view_column_new_with_attributes ("Sélectionné", renderer_toggle, "active", 0 , NULL);
	g_signal_connect (renderer_toggle, "toggled", G_CALLBACK (callback_profile_toggle), data);
	gtk_tree_view_append_column (tree_view, column);

	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes ("Nom du profil", renderer, "text", 1 , NULL);
	gtk_tree_view_append_column (tree_view, column);

	if(listProfile == NULL)
		return;

	//Fill with existing profiles
	if(listProfile->length){
		node_t * current = listProfile->head;
		while(current != NULL){
			gtk_list_store_append(model, &iter);
			gtk_list_store_set (model, &iter, 0, (Profile*)current->val == current_profile, 1, ((Profile*)current->val)->name, -1);

			current = current->next;
		}
		settings_window_fill_entries(data, (Profile*)listProfile->head->val);
		data->selected_profile_index = 0;
	}

	settings_window_set_all_fields_active(data, listProfile->length);
}

void open_settings_window(SGlobalData *data) {
	GtkWidget *window = NULL;

	window =  GTK_WIDGET (gtk_builder_get_object (data->builder, "SettingsWindow"));

	if(!gtk_widget_get_visible (window)) {
		gtk_widget_show_all (window);
	}
}

void callback_settings_window_close(GtkButton *widget, gpointer user_data) {
	GtkWidget *window = NULL;
	SGlobalData *data = (SGlobalData*) user_data;

	window =  GTK_WIDGET (gtk_builder_get_object (data->builder, "SettingsWindow"));
	gtk_widget_hide (window);

	tree_browsing_refresh(data);
}

gboolean callback_profile_context_menu(GtkWidget *tree_view, GdkEventButton *event, gpointer user_data) {
	SGlobalData *data = (SGlobalData*) user_data;
	GtkTreeModel *model = gtk_tree_view_get_model (GTK_TREE_VIEW(tree_view));
	gchar *string;
	GtkTreeIter iter;
	GtkWidget *menu;
	GtkWidget *menu_item_remove;
	Profile *profile;
	gint* i;

	menu = GTK_WIDGET(gtk_builder_get_object (data->builder, "ContextMenuProfile"));
	menu_item_remove = GTK_WIDGET(gtk_builder_get_object (data->builder, "MenuItemProfileDelete"));

	gtk_widget_set_sensitive(menu_item_remove, 0);

	if (event->type == GDK_BUTTON_PRESS  &&  event->button == 3) {

		GtkTreeSelection *selection;

		selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));

		if (gtk_tree_selection_count_selected_rows(selection)  <= 1) {
			GtkTreePath *path;

			/* Get tree path for row that was clicked */
			if (gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(tree_view),	(gint) event->x, (gint) event->y, &path, NULL, NULL, NULL)) {
				gtk_tree_model_get_iter(model, &iter, path);
				gtk_tree_model_get (model, &iter, 0, &string, -1);

				gtk_widget_set_sensitive(menu_item_remove, 1); //Enable "Delete" button if a row is selected

				//Update fields
				i = gtk_tree_path_get_indices (path);
				profile = (Profile*)linkedlist_get(listProfile, *i);
				if(profile == NULL) {
					window_show_error("Une erreur est survenue.\nLe profil sélectionné n'existe pas.", data, "SettingsWindow");
				} else {
					data->selected_profile_index = *i;
					settings_window_fill_entries(data, profile);
					settings_window_set_all_fields_active(data, TRUE);
				}
			} else {
				gtk_tree_selection_unselect_all(selection);
			}
		}
		gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL, (event != NULL) ? event->button : 0,	gdk_event_get_time((GdkEvent*)event));
	}
	return FALSE;
}

void callback_profile_create(GtkMenuItem *menuitem, gpointer user_data) {
	SGlobalData *data = (SGlobalData*) user_data;
	char *name = NULL;
	int i = 0;
	char iStr[12];
	gboolean ok = FALSE;
	int istrlen = 0;

	GtkTreeIter iter;
	GtkListStore *model;
	GtkTreeView *tree_view;

	tree_view = GTK_TREE_VIEW (gtk_builder_get_object (data->builder, "TreeViewProfiles"));
	model = GTK_LIST_STORE(gtk_tree_view_get_model(tree_view));

	while(name == NULL) {

		if(i > 0) {
			sprintf(iStr, "%d", i);
			istrlen = strlen(iStr);
		}

		name = malloc(14 + (istrlen > 0 ? istrlen+3 : 0)); //"Nom du profil"
		if(name == NULL) {
			window_show_error("Une erreur est survenue.\nMémoire insuffisante", data, "SettingsWindow");
			return;
		}
		strcpy(name, "Nom du profil");
		if(istrlen > 0) {
			strcat(name, " (");
			strcat(name, iStr);
			strcat(name, ")");
		}
		i++;

		ok = !check_if_name_exists(name);

		if(ok) {
			//Name found, exit the loop
			break;
		} else {
			free(name);
			name = NULL;
		}
	}

	if(name == NULL) {
		window_show_error("Une erreur est survenue.\nImpossible de trouver un nom disponible.", data, "SettingsWindow");
	} else {
		//Name found, create profile
		Profile *profile = initProfile();
		if(profile == NULL) {
			window_show_error("Une erreur est survenue.\nMémoire insuffisante", data, "SettingsWindow");
			return;
		}
		profile->name = name;

		linkedlist_add(listProfile,profile);
		saveProfile(profile, NULL);
		gtk_list_store_append(model, &iter);
		gtk_list_store_set (model, &iter, 0, FALSE, 1, profile->name, -1);
	}
}

void callback_profile_delete(GtkMenuItem *menuitem, gpointer user_data) {
	SGlobalData *data = (SGlobalData*) user_data;
	action = DELETE_PROFILE;
	show_confirm_dialog("Êtes-vous sûr de vouloir supprimer ce profil?\nCette action est irréversible.", data, "SettingsWindow");
}

/**
 * Gets the selected row in the profile tree and puts the reference to the value of the row in "string"
 */
void tree_profile_get_selected_row(SGlobalData *data, gchar **string, GtkTreeIter *iter) {
	GtkTreeModel *model;
	GtkWidget *tree_view;
	GtkTreeSelection *selection;

	tree_view = GTK_WIDGET(gtk_builder_get_object (data->builder, "TreeViewProfiles"));
	model = gtk_tree_view_get_model (GTK_TREE_VIEW(tree_view));
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));

	if (gtk_tree_selection_count_selected_rows(selection)  == 1) {

		/* Get selected row */
		gtk_tree_selection_get_selected (selection, &model, iter);
		gtk_tree_model_get (model, iter, 1, string, -1);

	}
}
