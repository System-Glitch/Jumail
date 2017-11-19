/**
 * 	MainWindow.c
 *
 *  Author : Jérémy LAMBERT
 *  Date : 19/11/2017
 *  Description : Creates and manage the main window
 */
#include "MainWindow.h"

/**
 * Initializes the main window
 */
static void main_window_activate(GtkApplication* app, gpointer data) {
	GtkWidget *window;

	window = gtk_application_window_new (app);
	gtk_window_set_title (GTK_WINDOW (window), "Jumail");
	gtk_window_set_default_size (GTK_WINDOW (window), 1000, 600);
	gtk_container_set_border_width (GTK_CONTAINER (window), 10);

	gtk_widget_show_all (window);
}

int main_window_start(int argc, char** argv) {
	GtkApplication *app;
	int status;

	app = gtk_application_new ("fr.sysgli.gtktest", G_APPLICATION_FLAGS_NONE);
	g_signal_connect (app, "activate", G_CALLBACK (main_window_activate), NULL);
	status = g_application_run (G_APPLICATION (app), argc, argv);
	g_object_unref (app);

	return status;
}
