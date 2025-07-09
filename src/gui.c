#include "gui.h"
#include "gtk/gtk.h"
#include "networking.h"
#include <stdio.h>

static void on_entry_activate(GtkEntry *entry, gpointer user_data) {
  AppContext *ctx = user_data;
  const char *text = gtk_editable_get_text(GTK_EDITABLE(entry));
  g_print("Entered text: %s\n", text);

  if (ctx->socket_fd != 1) {
#ifdef _WIN32
    int send_status = send(ctx->socket_fd, text, strlen(text), 0) == -1;
#else
    ssize_t send_status = write(ctx->socket_fd, text, strlen(text)) == -1;
#endif
  }else {
    perror("FD is not defined");
  }

  gtk_editable_set_text(GTK_EDITABLE(entry), "");
}

static void activate(GtkApplication *app, gpointer user_data) {
    AppContext *ctx = user_data;
    GtkBuilder *builder = gtk_builder_new();
    const char *ui_file = "./ui/window.ui";

    if (!gtk_builder_add_from_file(builder, ui_file, NULL)) {
        g_printerr("Failed to load UI file: %s\n", ui_file);
        return;
    }

    GtkWindow *window = GTK_WINDOW(gtk_builder_get_object(builder, "main_window"));
    gtk_window_set_application(window, app);

    GtkLabel *label = GTK_LABEL(gtk_builder_get_object(builder, "chat_label"));
    GtkEntry *entry = GTK_ENTRY(gtk_builder_get_object(builder, "chat_entry"));

    g_signal_connect(entry, "activate", G_CALLBACK(on_entry_activate), ctx);
    gtk_window_present(window);
    g_object_unref(builder);
}

GtkApplication *create_app(AppContext *ctx) {
    GtkApplication *app = gtk_application_new("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), ctx);
    return app;
}
