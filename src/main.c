/**
 * 	main.c
 *
 *  Author : Jérémy LAMBERT
 *  Date : 19/11/2017
 *  Description : The main file
 */
#include "gui/MainWindow.h"
#include "resources.h"

int main(int argc, char** argv) {
	putenv ("LANG=fr_FR");
	return main_window_start(argc, argv);
}


