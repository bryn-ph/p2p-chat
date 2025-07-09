#include "gui.h"
#include "networking.h"

int main(int argc, char **argv) {
    AppContext *ctx = g_malloc(sizeof(AppContext));
    connect_to_server(ctx, "0.0.0.0");
    GtkApplication *app = create_app(ctx);
    int status = g_application_run(G_APPLICATION(app), argc, argv);

    CLOSE(ctx->socket_fd);
    g_object_unref(app);
    g_free(ctx);

    return status;
}
