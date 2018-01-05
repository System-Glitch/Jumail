/**
 * 	SettingsWindow.c
 *
 *  Author : Jérémy LAMBERT
 *  Date : 05/01/2018
 *  Description : Creates and manage the settings window
 */
#include "SettingsWindow.h"
#include "../profils.h"

static gboolean clear_profile_selection_tree_view(GtkTreeModel *model, GtkTreePath  *path, GtkTreeIter *iter, gpointer user_data) {
	gtk_list_store_set(GTK_LIST_STORE(model), iter, 0, FALSE, -1);
	return FALSE;
}

static void settings_window_set_field_active(SGlobalData *data, const char * name, gboolean active) {
	GtkWidget *widget = GTK_WIDGET (gtk_builder_get_object (data->builder, name));
	gtk_widget_set_sensitive(widget, active);
}

static void settings_window_set_all_fields_active(SGlobalData *data, gboolean active) {
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

static void settings_window_fill_entries(SGlobalData *data, Profile *profile) {
	GtkCheckButton *check;

	settings_window_fill_entry(data, "ProfileEntryName", profile->name);
	settings_window_fill_entry(data, "ProfileEntryAddress", profile->emailAddress);
	settings_window_fill_entry(data, "ProfileEntryPassword", profile->password);
	settings_window_fill_entry(data, "ProfileEntryFullName", profile->fullName);
	settings_window_fill_entry(data, "ProfileEntryReceive", profile->receiveP);
	settings_window_fill_entry(data, "ProfileEntrySend", profile->sendP);

	/*check = GTK_CHECK_BUTTON (gtk_builder_get_object (data->builder, "ProfileCheckSSLReceive"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), !strcmp(profile->ssl_imap_enabled, "TRUE"));

	check = GTK_CHECK_BUTTON (gtk_builder_get_object (data->builder, "ProfileCheckSSLSend"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), !strcmp(profile->ssl_smtp_enabled, "TRUE"));

	check = GTK_CHECK_BUTTON (gtk_builder_get_object (data->builder, "ProfileCheckTLSSend"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), !strcmp(profile->tls_smtp_enabled, "TRUE"));*/

}

static gboolean check_if_name_exists(const char* name, Profile *exclusion) {
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

	return FALSE;
}

//Update the profile name in the list
void callback_profile_name_changed(GtkEditable *editable, gpointer user_data) {
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

	if (strlen(string) > 0 && !check_if_name_exists(string, profile)) {

		profile = (Profile*)linkedlist_get(listProfile, data->selected_profile_index);

		previous_name = malloc(strlen(profile->name)+1);
		profile->name = realloc(profile->name, strlen(string)+1);
		if(profile->name == NULL || previous_name == NULL) {
			window_show_error("Une erreur est survenue.\nMémoire insuffisante.", data, "SettingsWindow");
			return;
		}

		strcpy(previous_name, profile->name);
		strcpy(profile->name, string);
		//saveProfile(profile,previous_name);
		free(previous_name);

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

	tree_view = GTK_TREE_VIEW (gtk_builder_get_object (data->builder, "TreeViewProfiles"));
	model = GTK_LIST_STORE(gtk_tree_view_get_model(tree_view));
	tree_path = gtk_tree_path_new_from_string(path);
	i = gtk_tree_path_get_indices (tree_path);

	//Clear all selection
	gtk_tree_model_foreach (GTK_TREE_MODEL(model), clear_profile_selection_tree_view, NULL);

	//Set the selected one to "activated"
	gtk_tree_model_get_iter(GTK_TREE_MODEL(model), &iter, tree_path);
	gtk_list_store_set(model, &iter, 0, TRUE, -1);

	settings_window_set_all_fields_active(data, TRUE);
	//set_active_profile(i);

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
			gtk_list_store_set (model, &iter, 0, FALSE, 1, ((Profile*)current->val)->name, -1);
			current = current->next;
		}
		settings_window_fill_entries(data, (Profile*)listProfile->head->val);
		data->selected_profile_index = 0;
	}

	//TODO check selected profile
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
}
