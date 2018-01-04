/**
 * 	MailWindow.c
 *
 *  Author : Jérémy LAMBERT
 *  Date : 02/01/2018
 *  Description : Handler for the mail window
 */
#include "MailWindow.h"
#include "TreeBrowsing.h"

static void fill_text_view(GtkTextView *text_view, char *string) {
	GtkTextBuffer *buffer;
	buffer = gtk_text_view_get_buffer(text_view);
	gtk_text_buffer_set_text(buffer, string, -1);
}

static void fill_text_entry(GtkEntry *text_entry, char *string) {
	gtk_entry_set_text(text_entry, string);
}


/**
 * Opens a mail window showing the given mail
 */
int open_mail_window(Email *mail, char* mailbox, SGlobalData *data) {
	GtkWidget *window = NULL;
	GtkWidget *content = NULL;
	int uid;

	//TODO profile
	uid = ssl_search_by_id_with_new_connection("jumailimap@gmail.com", "azerty12", "imap.gmail.com", mailbox, mail->message_id);
	if(uid == -1) {
		window_show_error("Impossible de charger le message.\nVérifiez votre connexion internet et les paramètres de votre profil.", data, "MainWindow");
		return 0;
	} else if(uid == 0) {
		window_show_error("Une erreur est survenue.\nAucun message trouvé pour cet identifiant.", data, "MainWindow");
		return 0;
	}
	data->current_email = ssl_get_mail("jumailimap@gmail.com", "azerty12", "imap.gmail.com", mailbox, uid);

	window = GTK_WIDGET (gtk_builder_get_object (data->builder, "MailWindow"));
	gtk_window_set_title(GTK_WINDOW(window), mail->subject);
	gtk_widget_set_size_request (window, 800, 600);

	content =  GTK_WIDGET (gtk_builder_get_object (data->builder, "MailContent"));
	fill_text_view(GTK_TEXT_VIEW(content), data->current_email->message);
	content =  GTK_WIDGET (gtk_builder_get_object (data->builder, "MailFrom"));
	fill_text_view(GTK_TEXT_VIEW(content), data->current_email->from);
	content =  GTK_WIDGET (gtk_builder_get_object (data->builder, "MailTo"));
	fill_text_view(GTK_TEXT_VIEW(content), data->current_email->to);
	content =  GTK_WIDGET (gtk_builder_get_object (data->builder, "MailDate"));
	fill_text_view(GTK_TEXT_VIEW(content), data->current_email->date);
	content =  GTK_WIDGET (gtk_builder_get_object (data->builder, "MailSubject"));
	fill_text_view(GTK_TEXT_VIEW(content), data->current_email->subject);

	gtk_widget_hide(window);
	gtk_widget_show_all (window);
	return 1;
}

/**
 * Clears the mail window and close it
 */
void mail_window_clear(SGlobalData *data) {
	GtkWidget *window = NULL;

	window =  GTK_WIDGET (gtk_builder_get_object (data->builder, "MailWindow"));
	gtk_window_set_title(GTK_WINDOW(window), "Subject");

	free_email(data->current_email);
	if(data->current_email != NULL)
		free(data->current_email);
	data->current_email = NULL;

	gtk_widget_hide(window);
	data->selected_mail_index = -1;
}

/**
 * Opens the mail composing window
 */
void open_compose_mail_window(SGlobalData *data) {
	GtkWidget *window = NULL;
	GtkWidget *content = NULL;
	GtkWidget *button;

	button = GTK_WIDGET(gtk_builder_get_object (data->builder, "MailComposeSend"));
	window =  GTK_WIDGET (gtk_builder_get_object (data->builder, "MailComposeWindow"));

	if(!gtk_widget_get_visible (window)) {
		gtk_window_set_title(GTK_WINDOW(window), "Écrire");
		gtk_widget_set_size_request (window, 800, 600);
		g_signal_connect (window, "delete_event", G_CALLBACK (gtk_widget_hide), NULL);

		content =  GTK_WIDGET (gtk_builder_get_object (data->builder, "MailComposeTo"));
		fill_text_entry(GTK_ENTRY(content), "");
		content =  GTK_WIDGET (gtk_builder_get_object (data->builder, "MailComposeSubject"));
		fill_text_entry(GTK_ENTRY(content), "");
		content =  GTK_WIDGET (gtk_builder_get_object (data->builder, "MailComposeContent"));
		fill_text_view(GTK_TEXT_VIEW(content), "");

		gtk_widget_set_sensitive(button, 0);
		gtk_widget_show_all (window);
	}
}

void callback_mail_window_close(GtkWidget *widget, GdkEvent *event, gpointer user_data) {
	SGlobalData *data = (SGlobalData*) user_data;
	mail_window_clear(data);
}

void callback_show_mail(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data) {
	Email *mail;
	GtkTreeIter iter;
	GtkTreeModel *model;
	SGlobalData *data = (SGlobalData*) user_data;
	char *folder;
	int *i = gtk_tree_path_get_indices ( path );

	model = gtk_tree_view_get_model (GTK_TREE_VIEW(tree_view));

	mail = linkedlist_get(loaded_mails, *i);
	if(mail != NULL) {
		tree_browsing_get_selected_row(data, &folder, &iter);

		data->selected_mail_index = *i;
		if(open_mail_window(mail, folder ,data)) {
			list_folder_get_selected_row(data, &iter);
			gtk_list_store_set (GTK_LIST_STORE(model), &iter, 0, mail->subject, 1, mail->from, 2, mail->to, 3, mail->date, 4, PANGO_WEIGHT_NORMAL, 5, TRUE, -1);

		}
	}

}

void callback_compose_mail(GtkMenuItem *menuitem, gpointer user_data) {
	SGlobalData *data = (SGlobalData*) user_data;
	open_compose_mail_window(data);
}

void callback_compose_mail_to_entry_changed(GtkEditable *editable, gpointer user_data) {
	SGlobalData *data = (SGlobalData*) user_data;
	GtkWidget *button;

	button = GTK_WIDGET(gtk_builder_get_object (data->builder, "MailComposeSend"));

	//Disable the "confirm" button if the field is empty
	gtk_widget_set_sensitive(button, strlen(gtk_entry_get_text(GTK_ENTRY(editable))));

}

void callback_compose_mail_send(GtkToolButton *widget, gpointer user_data) {
	SGlobalData *data = (SGlobalData*) user_data;
	GtkEntry *entry_to;
	GtkEntry *entry_subject;
	GtkTextView *text_view;
	GtkTextIter start, end;
	GtkWidget *window;
	gchar *text;
	GtkWidget *button;
	int status;

	entry_to = GTK_ENTRY(gtk_builder_get_object (data->builder, "MailComposeTo"));
	entry_subject = GTK_ENTRY(gtk_builder_get_object (data->builder, "MailComposeSubject"));
	text_view = GTK_TEXT_VIEW(gtk_builder_get_object (data->builder, "MailComposeContent"));
	button = GTK_WIDGET(gtk_builder_get_object (data->builder, "MailComposeSend"));
	window =  GTK_WIDGET (gtk_builder_get_object (data->builder, "MailComposeWindow"));

	GtkTextBuffer *buffer = gtk_text_view_get_buffer (text_view);

	gtk_text_buffer_get_bounds (buffer, &start, &end);
	text = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);

	char * id = generate_id(); //Must generate a unique Message-ID for our mail
	if(id == NULL) {
		fprintf(stderr, "An error occured while getting a new GUID. Check your internet connection.\n");
		window_show_error("Une erreur est survenue lors de l'envoi du mail.\nVérifiez votre connexion internet.", data, "MailComposeWindow");
		return;
	}
	//TODO profile
	char** header = get_header("jumailimap@gmail.com", (char*)gtk_entry_get_text(entry_to), "User Name", (char*)gtk_entry_get_text(entry_subject), NULL, NULL, id); //Generate the header
	if(header == NULL) {
		fprintf(stderr, "An error occured while creating the email header.\n");
		free(id);
		window_show_error("Une erreur est survenue lors de l'envoi du mail.\nErreur lors de la génération des headers.", data, "MailComposeWindow");
		return;
	}
	free(id); //We don't need "id" anymore as it is copied into the header
	char ** mail = get_mail(header,text); //Generate the whole payload
	if(mail == NULL) {
		fprintf(stderr, "An error occured while creating the email payload.\n");
		free_header(header);
		window_show_error("Une erreur est survenue lors de l'envoi du mail.\nErreur lors de la génération du payload.", data, "MailComposeWindow");
		return;
	}

	status = send_mail_ssl("jumailimap@gmail.com", "azerty12", (char*)gtk_entry_get_text(entry_to), "smtp.gmail.com", (const char **)mail); //TODO Profile

	free_header(header); //The header is a two dimensional array. It must be freed using this function
	free_mail(mail); //Always free
	g_free(text);

	if(!status) {
		//Close compose window and popup success
		gtk_widget_set_sensitive(button, 0);
		gtk_widget_hide (window);
		window_show_info("Message envoyé !", data, "MainWindow");
	} else {
		window_show_error("Erreur lors de l'envoi du message.\nVérifiez les paramètres de votre profil.", data, "MailComposeWindow");
	}

}

void callback_about (GtkMenuItem *menuitem, gpointer user_data) {
	SGlobalData *data = (SGlobalData*) user_data;
	GtkWidget *dialog = NULL;

	dialog =  GTK_WIDGET (gtk_builder_get_object (data->builder, "AboutDialog"));
	gtk_dialog_run (GTK_DIALOG (dialog));

	gtk_widget_hide (dialog);
}

void callback_mail_view_delete_email(GtkButton *widget, gpointer user_data) {
	SGlobalData *data = (SGlobalData*) user_data;
	action = DELETE_MAIL_FROM_VIEW;
	show_confirm_dialog("Êtes-vous sûr de vouloir supprimer ce message?\nCette action est irréversible.", data, "MailWindow");
}

void callback_mail_view_move_email(GtkButton *widget, gpointer user_data) {
	SGlobalData *data = (SGlobalData*) user_data;
	action = MOVE_MAIL_FROM_VIEW;
	show_folder_select_dialog(data, "MailWindow");
}
