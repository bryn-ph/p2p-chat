#include "gui.h"
#include <gtk/gtk.h>

int main(int argc, char **argv) {
    GtkApplication *app = create_app();
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}