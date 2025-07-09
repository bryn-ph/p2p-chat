#include "gui.h"
#include "networking.h"

int main(int argc, char **argv) {
    AppContext *ctx = g_malloc(sizeof(AppContext));
    connect_to_server(ctx, "0.0.0.0");

    pthread_create(&ctx->listener_thread, NULL, listener_thread, ctx);

    GtkApplication *app = create_app(ctx);
    int status = g_application_run(G_APPLICATION(app), argc, argv);

    if (ctx->socket_fd >= -1) {
      CLOSE(ctx->socket_fd);
    }
    pthread_join(ctx->client_thread, NULL);
    pthread_join(ctx->listener_thread, NULL);
    //TODO: Handle multiple peer threads
    //Linked list in peer struct?
    g_object_unref(app);
    g_free(ctx);

    return status;
}
