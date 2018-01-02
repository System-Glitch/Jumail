/**
 * 	MailWindow.c
 *
 *  Author : Jérémy LAMBERT
 *  Date : 02/01/2018
 *  Description : Handler for the mail window
 */
#include "MailWindow.h"

static void fill_text_view(GtkTextView *text_view, char *string) {
	GtkTextBuffer *buffer;
	buffer = gtk_text_view_get_buffer(text_view);
	gtk_text_buffer_set_text(buffer, string, -1);
}


/**
 * Opens a mail window showing the given mail
 */
void open_mail_window(Email *mail, SGlobalData *data) {
	GtkWidget *window = NULL;
	GtkWidget *content = NULL;


	window =  GTK_WIDGET (gtk_builder_get_object (data->builder, "MailWindow"));
	gtk_window_set_title(GTK_WINDOW(window), mail->subject);
	gtk_widget_set_size_request (window, 800, 600);
	g_signal_connect (window, "delete_event", G_CALLBACK (gtk_widget_hide), NULL);

	content =  GTK_WIDGET (gtk_builder_get_object (data->builder, "MailContent"));
	fill_text_view(GTK_TEXT_VIEW(content), mail->message);
	content =  GTK_WIDGET (gtk_builder_get_object (data->builder, "MailFrom"));
	fill_text_view(GTK_TEXT_VIEW(content), mail->from);
	content =  GTK_WIDGET (gtk_builder_get_object (data->builder, "MailTo"));
	fill_text_view(GTK_TEXT_VIEW(content), mail->to);
	content =  GTK_WIDGET (gtk_builder_get_object (data->builder, "MailDate"));
	fill_text_view(GTK_TEXT_VIEW(content), mail->date);
	content =  GTK_WIDGET (gtk_builder_get_object (data->builder, "MailSubject"));
	fill_text_view(GTK_TEXT_VIEW(content), mail->subject);

	gtk_widget_hide(window);
	gtk_widget_show_all (window);
}
