#include "networking.h"
#include <netinet/in.h>
#include <pthread.h>
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
  #include <arpa/inet.h>
#endif

volatile sig_atomic_t stop = 0;
void handle_sigint(int sig) {
  (void)sig;
  stop = 1;
  printf("\nCaught Ctrl+C. Exiting...\n");
}

static void* client_thread(void* arg) {
  AppContext* ctx = (AppContext*)arg;

  // Write data to server
  char buffer[1024];
  
  while (!stop) {
    // Check if server is still connected
    ssize_t server_msg = recv(ctx->socket_fd, buffer, sizeof(buffer) - 1, 0);

    if (server_msg > 0 ) {
      buffer[server_msg] = '\0';
      buffer[strcspn(buffer, "\r\n")] = '\0';
      printf("Received from client: %s\n", buffer);
    } else if (server_msg == 0) {
      // Server closed the connection gracefully
      printf("Server disconnected (gracefully)\n");
      break;
    } else {
#ifdef _WIN32
      int err = WSAGetLastError();
      if (err == WSAEWOULDBLOCK) {
        // No data available
        usleep(100000);
        continue;
      } else {
        printf("recv failed with error: %d\n", err);
        break;
      }
#else
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        // No data available
        usleep(100000);  // Sleep 100ms to prevent busy loop
        continue;
      } else if (errno == EINTR) {
        // Interrupted by signal, try again
        continue;
      } else {
        perror("read failed");
        break;
      }
#endif
    }
  }

  CLOSE(ctx->socket_fd);
  ctx->socket_fd = -1;
  return NULL;
}

static void* peer_handler_thread(void *arg) {
  PeerConnection *peer = (PeerConnection*)arg;
  char buffer[1024];

  printf("New peer connected from %s:%d\n",
      inet_ntoa(peer->peer_addr.sin_addr),
      ntohs(peer->peer_addr.sin_port));

  while (!stop) {
    ssize_t receive_status = recv(peer->socket_fd, buffer, sizeof(buffer) - 1, 0);

    if (receive_status > 0) {
      buffer[receive_status] = '\0';
      buffer[strcspn(buffer, "\r\n")] = '\0';
      printf("%s:%d: %s\n",
          inet_ntoa(peer->peer_addr.sin_addr),
          ntohs(peer->peer_addr.sin_port),
          buffer);
    }else if (receive_status == 0) {
      printf("%s:%d disconnected\n",
          inet_ntoa(peer->peer_addr.sin_addr),
          ntohs(peer->peer_addr.sin_port));
      break;
    }else {
      // Handles both sigint and error
#ifdef _WIN32
      int err = WSAGetLastError();
      if (err == WSAEWOULDBLOCK) {
        // No data available
        usleep(100000);
        continue;
      } else {
        printf("recv failed with error: %d\n", err);
        break;
      }
#else
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        // No data available
        usleep(100000);  // Sleep 100ms to prevent busy loop
        continue;
      } else if (errno == EINTR) {
        // Interrupted by signal, try again
        continue;
      } else {
        perror("read failed");
        break;
      }
#endif
    }
  }

  CLOSE(peer->socket_fd);
  free(peer);
  return NULL;
}
void* listener_thread(void *arg) {
  AppContext *ctx = (AppContext*)arg;

#ifdef _WIN32
  WSADATA wsaData;
  if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
    printf("WSAStartup failed\n");
    exit(EXIT_FAILURE);
  }
#endif

  struct sockaddr_in addr;

  ctx->listening_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (ctx->listening_fd == -1) {
    perror("Kappa Chungus this is not netWORKING\n");
    exit(EXIT_FAILURE);
  }

  memset(&addr, 0, sizeof(addr));  
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(8081);

  if (bind(ctx->listening_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
    perror("Kappa Chungus this is not netWORKING BIND\n");
    CLOSE(ctx->listening_fd);
    exit(EXIT_FAILURE);
  }

  // Listen for incoming connections
  if (listen(ctx->listening_fd, 5) == -1) {
    printf("OOFT cannot listen\n");
    CLOSE(ctx->listening_fd);
    exit(EXIT_FAILURE);
  }

  printf("Listening on port 8080\n");

  while (!stop) {
    PeerConnection* peer = malloc(sizeof(PeerConnection));
    if (!peer) {
      perror("malloc");
      continue;
    }

    peer->addr_len = sizeof(peer->peer_addr);
    SOCKET_TYPE peer_fd = accept(ctx->listening_fd, (struct sockaddr *)&peer->peer_addr, &peer->addr_len);
    if (peer_fd == -1) {
      perror("failed to accept peer");
      free(peer);
      continue;
    }

    peer->socket_fd = peer_fd;
    pthread_create(&peer->thread_id, NULL, peer_handler_thread, peer);
  }

  CLOSE(ctx->listener_thread);
  return NULL;
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

  if (pthread_create(&ctx->client_thread, NULL, client_thread, ctx) != 0) {
    perror("Failed to start client thread");
    CLOSE(fd);
    exit(EXIT_FAILURE);
  }
}
