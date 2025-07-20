#include "gui.h"
#include "networking.h"

int main(int argc, char **argv) {
    AppContext *ctx = g_malloc(sizeof(AppContext));

    pthread_create(&ctx->listener_thread, NULL, listener_thread, ctx);

    GtkApplication *app = create_app(ctx);
    ctx->app = app;
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_print("closing\n");

    if (ctx->socket_fd >= -1) {
      g_print("CLOSING SOCKET\n");
      CLOSE(ctx->socket_fd);
    }

    g_print("SOCKET IS CLOSED\n");
    if (ctx->client_thread) {
      pthread_join(ctx->client_thread, NULL);
    }
    if (ctx->listener_thread) {
      pthread_join(ctx->listener_thread, NULL);
    }
    //TODO: Handle multiple peer threads
    //Linked list in peer struct?
    g_object_unref(app);
    g_free(ctx);

    g_print("We shilling\n");

    return status;
}
