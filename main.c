#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>

static void print_hello (GtkWidget *widget, gpointer data) {
  g_print("Hello World\n");
}

static void activate(GtkApplication *app, gpointer user_data) {
  GtkBuilder *builder = gtk_builder_new();
  const char *ui_file = "./window.ui";

  if (!gtk_builder_add_from_file(builder, ui_file, NULL)) {
      g_printerr("Failed to load UI file: %s\n", ui_file);
      return;
  }

  GtkWindow *window;
  GtkButton *button;

  window = GTK_WINDOW(gtk_builder_get_object(builder, "main_window"));
  gtk_window_set_application(window, app);

  button = GTK_BUTTON(gtk_builder_get_object(builder, "click_button"));
  g_signal_connect(button, "clicked", G_CALLBACK(print_hello), NULL);

  gtk_window_present(window);
  g_object_unref(builder);
}

int main (int argc, char **argv) {
  GtkApplication *app;
  int status;

  app = gtk_application_new("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);

  return status;
}
