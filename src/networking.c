#include "networking.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#ifdef _WIN32
  #include <ws2tcpip.h>
#else
  #include <fcntl.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
#endif

volatile sig_atomic_t stop = 0;
void handle_sigint(int sig) {
  (void)sig;
  stop = 1;
  printf("\nCaught Ctrl+C. Exiting...\n");
}

void connect_to_server(AppContext *ctx, char *addr) {
#ifdef _WIN32
  WSADATA wsaData;
  if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
    printf("WSAStartup failed\n");
    exit(EXIT_FAILURE);
  }
#endif

  struct sockaddr_in server;
  SOCKET_TYPE fd;

  printf("Arg passed is: %s\n", addr);

  fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd == -1) {
    perror("Kappa Chungus this is not netWORKING");
    exit(EXIT_FAILURE);
  }

  memset(&server, 0, sizeof(server));  
  server.sin_family = AF_INET;
  server.sin_port = htons(8080);
  if (inet_pton(AF_INET, addr, &server.sin_addr) <= 0) {
    perror("Invalid address");
    exit(EXIT_FAILURE);
  }

  // Connect to server
  printf("Trying to connect to server\n");
  socklen_t addrlen = sizeof(server);
  if (connect(fd, (struct sockaddr *) &server, addrlen) == -1) {
    perror("OOFT cannot connec");
    CLOSE(fd);
    exit(EXIT_FAILURE);
  }

  printf("Connected to %s on port 8080\n", addr);

   // Set client socket to non-blocking (can be interrupted by SIGINT etc...)
#ifdef _WIN32
    u_long mode = 1;  // 1 = non-blocking
    if (ioctlsocket(fd, FIONBIO, &mode) != 0) {
      printf("Failed to set non-blocking mode\n");
      CLOSE(fd);
      exit(EXIT_FAILURE);
  }
#else
  int flags = fcntl(fd, F_GETFL, 0);
  if (flags == -1) {
    perror("fcntl get");
    CLOSE(fd);
    exit(EXIT_FAILURE);
  }
  if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
    perror("fcntl set");
    CLOSE(fd);
    exit(EXIT_FAILURE);
  }
#endif

  // Signal Handling
#ifdef _WIN32
  signal(SIGINT, handle_sigint);
#else
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = handle_sigint;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGINT, &sa, NULL);
#endif

  ctx->socket_fd = fd;
}
