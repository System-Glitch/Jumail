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
	gchar *utf8 = NULL;
	size_t len = strlen(string);
	GError *error = NULL;

	buffer = gtk_text_view_get_buffer(text_view);
	if(g_utf8_validate(string, len, NULL)) {
		gtk_text_buffer_set_text(buffer, string, len);
	} else {
		utf8 = g_convert(string, len, "UTF-8", "iso-8859-15", NULL, NULL, &error);
		if (error != NULL) {
			fputs ("Couldn't text to UTF-8\n",stderr);
			g_error_free (error);
		} else {
			gtk_text_buffer_set_text(buffer, utf8, len);
			g_free(utf8);
		}
	}
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
	data->is_archived = FALSE;

	if(!check_selected_profile(data, "MainWindow")) return 0;

	uid = ssl_search_by_id_with_new_connection(current_profile->emailAddress, current_profile->password, current_profile->receiveP, mailbox, mail->message_id, strequals(current_profile->SslImap, "TRUE"));
	if(uid == -1) {
		window_show_error("Impossible de charger le message.\nVérifiez votre connexion internet et les paramètres de votre profil.", data, "MainWindow");
		return 0;
	} else if(uid == 0) {
		window_show_error("Une erreur est survenue.\nAucun message trouvé pour cet identifiant.", data, "MainWindow");
		return 0;
	}
	data->current_email = ssl_get_mail(current_profile->emailAddress, current_profile->password, current_profile->receiveP, mailbox, strequals(current_profile->SslImap, "TRUE"), uid);

	window = GTK_WIDGET (gtk_builder_get_object (data->builder, "MailWindow"));
	gtk_window_set_title(GTK_WINDOW(window), mail->subject);
	gtk_widget_set_size_request (window, 800, 600);
	gtk_window_set_transient_for(GTK_WINDOW(window), GTK_WINDOW (gtk_builder_get_object (data->builder, "MainWindow")));

	if(data->current_email == NULL) {
		gtk_widget_hide(window);
		return 0;
	}

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

	gtk_widget_set_sensitive(GTK_WIDGET (gtk_builder_get_object (data->builder, "MailViewArchive")), TRUE);

	gtk_widget_hide(window);
	gtk_widget_show_all (window);
	return 1;
}

/**
 * Opens a mail window showing the given mail
 */
int open_mail_window_from_file(Email *mail, SGlobalData *data) {
	GtkWidget *window = NULL;
	GtkWidget *content = NULL;

	data->is_archived = TRUE;
	data->current_email = mail;

	window = GTK_WIDGET (gtk_builder_get_object (data->builder, "MailWindow"));
	gtk_window_set_title(GTK_WINDOW(window), mail->subject);
	gtk_widget_set_size_request (window, 800, 600);
	gtk_window_set_transient_for(GTK_WINDOW(window), GTK_WINDOW(gtk_builder_get_object (data->builder, "ArchivesWindow")));

	if(data->current_email == NULL) {
		gtk_widget_hide(window);
		return 0;
	}

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

	gtk_widget_set_sensitive(GTK_WIDGET (gtk_builder_get_object (data->builder, "MailViewArchive")), FALSE);

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

	window =  GTK_WIDGET (gtk_builder_get_object (data->builder, "MailRawWindow"));
	gtk_widget_hide(window);
}

/**
 * Opens the mail composing window
 */
void open_compose_mail_window(SGlobalData *data) {
	GtkWidget *window = NULL;
	GtkWidget *content = NULL;
	GtkWidget *button;

	if(!check_selected_profile()) {
		window_show_info("Vous devez sélectionner un profil avant de\npouvoir envoyer des messages.", data, "MainWindow");
		return;
	}

	button = GTK_WIDGET(gtk_builder_get_object (data->builder, "MailComposeSend"));
	window =  GTK_WIDGET (gtk_builder_get_object (data->builder, "MailComposeWindow"));

	if(!gtk_widget_get_visible (window)) {
		gtk_window_set_title(GTK_WINDOW(window), "Écrire");
		gtk_widget_set_size_request (window, 800, 600);

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
	int *i = gtk_tree_path_get_indices ( path );

	model = gtk_tree_view_get_model (GTK_TREE_VIEW(tree_view));

	mail = linkedlist_get(loaded_mails, *i);
	if(mail != NULL) {
		data->selected_mail_index = *i;
		if(open_mail_window(mail, data->selected_folder ,data)) {
			list_folder_get_selected_row(data, &iter);
			gtk_list_store_set (GTK_LIST_STORE(model), &iter, 0, mail->subject, 1, mail->from, 2, mail->to, 3, mail->date, 4, PANGO_WEIGHT_NORMAL, 5, TRUE, -1);
		} else {
			window_show_error("Une erreur est survenue.\nImpossible de récupérer le message.", data, "MainWindow");
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
	char **header;
	char *id;
	char **mail;

	if(!check_selected_profile(data, "MailComposeWindow")) return;

	entry_to = GTK_ENTRY(gtk_builder_get_object (data->builder, "MailComposeTo"));
	entry_subject = GTK_ENTRY(gtk_builder_get_object (data->builder, "MailComposeSubject"));
	text_view = GTK_TEXT_VIEW(gtk_builder_get_object (data->builder, "MailComposeContent"));
	button = GTK_WIDGET(gtk_builder_get_object (data->builder, "MailComposeSend"));
	window =  GTK_WIDGET (gtk_builder_get_object (data->builder, "MailComposeWindow"));

	GtkTextBuffer *buffer = gtk_text_view_get_buffer (text_view);

	gtk_text_buffer_get_bounds (buffer, &start, &end);
	text = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);

	id = generate_id(); //Must generate a unique Message-ID for our mail
	if(id == NULL) {
		fprintf(stderr, "An error occured while getting a new GUID. Check your internet connection.\n");
		window_show_error("Une erreur est survenue lors de l'envoi du mail.\nVérifiez votre connexion internet.", data, "MailComposeWindow");
		return;
	}

	if(action == RESPOND_MAIL_FROM_VIEW) {
		header = get_header(current_profile->emailAddress,
				(char*)gtk_entry_get_text(entry_to),
				current_profile->fullName,
				(char*)gtk_entry_get_text(entry_subject),
				data->response_reference->message_id,
				data->response_reference->references == NULL ? data->response_reference->in_reply_to : data->response_reference->references,
						id); //Generate the header
	} else {
		header = get_header(current_profile->emailAddress, (char*)gtk_entry_get_text(entry_to), current_profile->fullName, (char*)gtk_entry_get_text(entry_subject), NULL, NULL, id); //Generate the header
	}
	if(header == NULL) {
		fprintf(stderr, "An error occured while creating the email header.\n");
		free(id);
		window_show_error("Une erreur est survenue lors de l'envoi du mail.\nErreur lors de la génération des headers.", data, "MailComposeWindow");
		return;
	}
	free(id); //We don't need "id" anymore as it is copied into the header
	mail = get_mail(header,text); //Generate the whole payload
	if(mail == NULL) {
		fprintf(stderr, "An error occured while creating the email payload.\n");
		free_header(header);
		window_show_error("Une erreur est survenue lors de l'envoi du mail.\nErreur lors de la génération du payload.", data, "MailComposeWindow");
		return;
	}

	status = send_mail_ssl(current_profile->emailAddress, current_profile->password, (char*)gtk_entry_get_text(entry_to), current_profile->sendP, strequals(current_profile->SslSmtp, "TRUE"), strequals(current_profile->TlsSmtp, "TRUE"), (const char **)mail);

	free_header(header); //The header is a two dimensional array. It must be freed using this function
	free_mail(mail); //Always free
	g_free(text);

	if(!status) {
		//Close compose window and popup success
		gtk_widget_set_sensitive(button, 0);
		gtk_widget_hide (window);
		free_email(data->response_reference);
		if(data->response_reference != NULL)
			free(data->response_reference);
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

static Email *clone_email_for_reference(Email *email) {
	Email *clone = init_email();

	if(clone == NULL) return NULL;

	clone->from = malloc(strlen(email->from)+1);
	if(clone->from == NULL) { free(clone); return NULL; }
	strcpy(clone->from, email->from);

	clone->subject = malloc(strlen(email->subject)+1);
	if(clone->subject == NULL) { free_email(clone); free(clone); return NULL; }
	strcpy(clone->subject, email->subject);

	clone->message_id = malloc(strlen(email->message_id)+1);
	if(clone->message_id == NULL) { free_email(clone); free(clone); return NULL; }
	strcpy(clone->message_id, email->message_id);

	if(email->in_reply_to != NULL) {
		clone->in_reply_to = malloc(strlen(email->in_reply_to)+1);
		if(clone->in_reply_to == NULL) { free_email(clone); free(clone); return NULL; }
		strcpy(clone->in_reply_to, email->in_reply_to);
	}

	if(email->references != NULL) {
		clone->references = malloc(strlen(email->references)+1);
		if(clone->references == NULL) { free_email(clone); free(clone); return NULL; }
		strcpy(clone->references, email->references);
	}

	return clone;
}

void callback_mail_view_response(GtkButton *widget, gpointer user_data) {
	SGlobalData *data = (SGlobalData*) user_data;
	GtkWidget *window = NULL;
	GtkWidget *content = NULL;
	GtkWidget *button;
	char *subject;

	free_email(data->response_reference);
	if(data->response_reference != NULL)
		free(data->response_reference);
	data->response_reference = clone_email_for_reference(data->current_email);

	if(data->response_reference == NULL) {
		window_show_error("Une erreur est survenue.\nMémoire insuffisante.", data, "MailWindow");
		return;
	}

	action = RESPOND_MAIL_FROM_VIEW;
	button = GTK_WIDGET(gtk_builder_get_object (data->builder, "MailComposeSend"));
	window =  GTK_WIDGET (gtk_builder_get_object (data->builder, "MailComposeWindow"));

	if(!gtk_widget_get_visible (window)) {

		if(!check_selected_profile()) {
			window_show_info("Vous devez sélectionner un profil avant de\npouvoir envoyer des messages.", data, "MainWindow");
			return;
		}
		open_compose_mail_window(data);
		subject = malloc(strlen(data->response_reference->subject)+5);

		strcpy(subject, "Re: ");
		strcat(subject, data->response_reference->subject);

		content =  GTK_WIDGET (gtk_builder_get_object (data->builder, "MailComposeTo"));
		fill_text_entry(GTK_ENTRY(content), data->response_reference->from);
		content =  GTK_WIDGET (gtk_builder_get_object (data->builder, "MailComposeSubject"));
		fill_text_entry(GTK_ENTRY(content), subject);
		gtk_widget_set_sensitive(button, 1);

		free(subject);
	}
}

void callback_mail_compose_window_close(GtkWidget *widget, GdkEvent *event, gpointer user_data) {
	SGlobalData *data = (SGlobalData*) user_data;
	free_email(data->response_reference);
	if(data->response_reference != NULL)
		free(data->response_reference);
	data->response_reference = NULL;
	gtk_widget_hide(widget);
}

void callback_mail_view_see_raw(GtkButton *widget, gpointer user_data) {
	SGlobalData *data = (SGlobalData*) user_data;
	GtkWidget *window = NULL;
	GtkWidget *content = NULL;

	window =  GTK_WIDGET (gtk_builder_get_object (data->builder, "MailRawWindow"));
	content =  GTK_WIDGET (gtk_builder_get_object (data->builder, "MailRawContent"));
	fill_text_view(GTK_TEXT_VIEW(content), data->current_email->raw);
	gtk_window_set_title(GTK_WINDOW(window), "Contenu original");

	gtk_widget_show_all(window);
}

void callback_mail_archive_from_view(GtkMenuItem *menuitem, gpointer user_data) {
	SGlobalData *data = (SGlobalData*) user_data;
	action = ARCHIVE_MAIL_FROM_VIEW;

	show_folder_select_dialog(data, "MailWindow");
}
