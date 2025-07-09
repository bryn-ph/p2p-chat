#ifndef NETWORKING_H
#define NETWORKING_H

#ifdef _WIN32
  #include <winsock2.h>
  #define SOCKET_TYPE SOCKET
  #define CLOSE closesocket
#else
  #include <pthread.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #define SOCKET_TYPE int
  #define CLOSE close
#endif
#include <signal.h>

typedef struct {
  SOCKET_TYPE socket_fd;
  pthread_t client_thread;
  SOCKET_TYPE listening_fd;
  pthread_t listener_thread;
} AppContext;

typedef struct {
  SOCKET_TYPE socket_fd;
  pthread_t thread_id;
  struct sockaddr_in peer_addr;
  socklen_t addr_len;
} PeerConnection;

extern volatile sig_atomic_t stop;

void connect_to_server(AppContext *ctx, char *argAddr);
void* listener_thread(void* arg);

#endif
