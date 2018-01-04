/**
 * 	MainWindow.c
 *
 *  Author : Jérémy LAMBERT
 *  Date : 19/11/2017
 *  Description : Creates and manage the main window
 */
#include "MainWindow.h"
#include "TreeBrowsing.h"
#include "../mailing.h"
#include "../profils.h"

/**
 * Initializes the main window
 */
static void main_window_activate(GtkApplication* app, gpointer user_data) {
	GtkWidget *main_window = NULL;
	GError *error = NULL;
	gchar *filename = NULL;
	SGlobalData data;

	data.builder = gtk_builder_new();
	data.current_email = NULL;
	data.selected_mail_index = -1;


	filename =  g_build_filename ("resources/MainWindow.glade", NULL);

	gtk_builder_add_from_file (data.builder, filename, &error);
	g_free (filename);
	if (error)
	{
		g_printerr("%s\n", error->message);
		g_error_free (error);
		return;
	}

	main_window = GTK_WIDGET(gtk_builder_get_object (data.builder, "MainWindow"));
	gtk_widget_set_size_request (main_window, 800, 600);

	g_signal_connect (app, "activate", G_CALLBACK (main_window_activate), NULL);
	g_signal_connect (G_OBJECT (main_window), "destroy", (GCallback)gtk_main_quit, NULL);
	gtk_builder_connect_signals (data.builder, &data);

	gtk_widget_show_all (main_window);

	tree_browsing_refresh(&data); //Fill the browser
	browsing_refresh_folder(NULL, &data);
	gtk_main();
}

static int init_app(int *argc, char*** argv) {
	GtkApplication *app;
	int status;

	fputs("Loading profiles...\n", stdout);
	loadAllProfile(); //Loading profiles

	//Loading GUI
	fputs("Loading GUI...\n", stdout);
	gtk_init(argc, argv);

	app = gtk_application_new ("fr.sysgli.jumail", G_APPLICATION_FLAGS_NONE);
	g_signal_connect (app, "activate", G_CALLBACK (main_window_activate), NULL);
	status = g_application_run (G_APPLICATION (app), *argc, *argv);
	g_object_unref (app);

	return status;
}

static void cleanup() {
	fputs("Shutting down...\n", stdout);
	fputs("Cleaning profiles...\n", stdout);
	freeListProfile();
	fputs("Cleaning loaded mails...\n", stdout);
	free_list_loaded_mails();
}

int main_window_start(int argc, char** argv) {

	int status;

	status = init_app(&argc, &argv);

	cleanup();

	return status;
}
