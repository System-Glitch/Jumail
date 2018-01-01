/**
 * 	MainWindow.h
 *
 *  Author : Jérémy LAMBERT
 *  Date : 19/11/2017
 *  Description : Creates and manage the main window
 */
#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <stdio.h>
#include <stdlib.h>
#include<gtk/gtk.h>

typedef struct
{
	GtkBuilder *builder;
	gpointer user_data;
} SGlobalData;

/**
 * Starts the program. Returns the frame exit code.
 */
int main_window_start(int argc, char** argv);

#endif
