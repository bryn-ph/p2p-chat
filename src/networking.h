#ifndef NETWORKING_H
#define NETWORKING_H

#ifdef _WIN32
  #include <winsock2.h>
  #include <ws2tcpip.h>
  #define SOCKET_TYPE SOCKET
  #define CLOSE closesocket
#else
  #include <pthread.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #define SOCKET_TYPE int
  #define CLOSE close
#endif
#include <gtk/gtk.h>
#include <signal.h>

typedef struct {
  SOCKET_TYPE socket_fd;
  SOCKET_TYPE listening_fd;
#ifdef _WIN32
  HANDLE client_thread;
  HANDLE listener_thread;
#else
  pthread_t client_thread;
  pthread_t listener_thread;
#endif
  GtkApplication * app;
} AppContext;

typedef struct {
  SOCKET_TYPE socket_fd;
#ifdef _WIN32
  HANDLE thread_id;
#else
  pthread_t thread_id;
#endif
  struct sockaddr_in peer_addr;
  socklen_t addr_len;
} PeerConnection;

extern volatile sig_atomic_t stop;

void connect_to_server(AppContext *ctx, char *argAddr);
void* listener_thread(void* arg);

#endif
