#ifndef NETWORKING_H
#define NETWORKING_H

#ifdef _WIN32
  #include <winsock2.h>
  #define SOCKET_TYPE SOCKET
  #define CLOSE closesocket
#else
  #define SOCKET_TYPE int
  #define CLOSE close
#endif

typedef struct {
  SOCKET_TYPE socket_fd;
} AppContext;

void connect_to_server(AppContext *ctx, char *argAddr);

#endif
