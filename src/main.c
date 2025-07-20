#include "gui.h"
#include "networking.h"

int main(int argc, char **argv) {
    AppContext *ctx = g_malloc(sizeof(AppContext));

#ifdef _WIN32
    ctx->listener_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)listener_thread, ctx, 0, NULL);
#else
    pthread_create(&ctx->listener_thread, NULL, listener_thread, ctx);
#endif
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
#ifdef _WIN32
      WaitForSingleObject(ctx->client_thread, 0);
#else
      pthread_join(ctx->client_thread, NULL);  
#endif
    }
    if (ctx->listener_thread) {
#ifdef _WIN32
      WaitForSingleObject(ctx->listener_thread, 0);
#else
      pthread_join(ctx->listener_thread, NULL);  
#endif
    }



    //TODO: Handle multiple peer threads
    //Linked list in peer struct?
    g_object_unref(app);
    g_free(ctx);

    g_print("We shilling\n");

    return status;
}
