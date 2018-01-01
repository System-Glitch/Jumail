/**
 * 	MainWindow.c
 *
 *  Author : Jérémy LAMBERT
 *  Date : 19/11/2017
 *  Description : Creates and manage the main window
 */
#include "MainWindow.h"


void callback_about (GtkMenuItem *menuitem, gpointer user_data) {
	SGlobalData *data = (SGlobalData*) user_data;
	GtkWidget *dialog = NULL;

	dialog =  GTK_WIDGET (gtk_builder_get_object (data->builder, "AboutDialog"));
	gtk_dialog_run (GTK_DIALOG (dialog));

	gtk_widget_hide (dialog);
}

/**
 * Initializes the main window
 */
static void main_window_activate(GtkApplication* app, gpointer user_data) {
	GtkWidget *main_window = NULL;
	GError *error = NULL;
	gchar *filename = NULL;
	SGlobalData data;

	data.builder = gtk_builder_new();
	data.user_data = "Test\n";


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

	g_signal_connect (app, "activate", G_CALLBACK (main_window_activate), NULL);
	g_signal_connect (G_OBJECT (main_window), "destroy", (GCallback)gtk_main_quit, NULL);
	gtk_builder_connect_signals (data.builder, &data);

	gtk_widget_show_all (main_window);

	gtk_main();
}

int main_window_start(int argc, char** argv) {

	GtkApplication *app;
	int status;

	gtk_init(&argc, &argv);

	app = gtk_application_new ("fr.sysgli.jumail", G_APPLICATION_FLAGS_NONE);
	g_signal_connect (app, "activate", G_CALLBACK (main_window_activate), NULL);
	status = g_application_run (G_APPLICATION (app), argc, argv);
	g_object_unref (app);

	return status;
}
