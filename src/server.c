#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#ifdef _WIN32
  #include <winsock2.h>
  #include <ws2tcpip.h>
  #define SOCKET_TYPE SOCKET
  #define CLOSE closesocket
#else
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #define SOCKET_TYPE int
  #define CLOSE close
#endif

volatile sig_atomic_t stop = 0;
void handle_sigint(int sig) {
  (void)sig;
  stop = 1;
  printf("\nCaught Ctrl+C. Exiting...\n");
}

int main() {
#ifdef _WIN32
  WSADATA wsaData;
  if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
    printf("WSAStartup failed\n");
    exit(EXIT_FAILURE);
  }
#endif

  SOCKET_TYPE fd;
  struct sockaddr_in addr;

  fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd == -1) {
    printf("Kappa Chungus this is not netWORKING\n");
    exit(EXIT_FAILURE);
  }

  memset(&addr, 0, sizeof(addr));  
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(8080);

  if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
    printf("Kappa Chungus this is not netWORKING BIND\n");
    CLOSE(fd);
    exit(EXIT_FAILURE);
  }

  // Listen for incoming connections
  if (listen(fd, 5) == -1) {
    printf("OOFT cannot listen\n");
    CLOSE(fd);
    exit(EXIT_FAILURE);
  }

  printf("Listening on port 8080\n");

  // Accept incoming connection
  struct sockaddr_in client_addr;
  socklen_t client_len = sizeof(client_addr);
  SOCKET_TYPE client_fd = accept(fd, (struct sockaddr *)&client_addr, &client_len);
  if (client_fd == -1) {
    printf("shocka client_fd fail\n");
    CLOSE(fd);
    exit(EXIT_FAILURE);
  }

  printf("Client connected!\n");

  // Set client socket to non-blocking (can be interrupted by SIGINT etc...)
#ifdef _WIN32
  u_long mode = 1;  // 1 = non-blocking
  if (ioctlsocket(client_fd, FIONBIO, &mode) != 0) {
    printf("Failed to set non-blocking mode\n");
    CLOSE(client_fd);
    CLOSE(fd);
    exit(EXIT_FAILURE);
  }
#else
  int flags = fcntl(client_fd, F_GETFL, 0);
  if (flags == -1) {
    perror("fcntl get");
    CLOSE(client_fd);
    CLOSE(fd);
    exit(EXIT_FAILURE);
  }
  if (fcntl(client_fd, F_SETFL, flags | O_NONBLOCK) == -1) {
    perror("fcntl set");
    CLOSE(client_fd);
    CLOSE(fd);
    exit(EXIT_FAILURE);
  }
#endif

  // Signal handling
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

  // Read msg from client
  char buffer[1024];
  int receive_status;

  while (!stop) {
    receive_status = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

    if (receive_status > 0) {
      buffer[receive_status] = '\0';
      buffer[strcspn(buffer, "\r\n")] = '\0';
      printf("Received from client: %s\n", buffer);
    }else if (receive_status == 0) {
      printf("Client disconnected\n");
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

// WSA cleanup for Windows
#ifdef _WIN32
  WSACleanup();
#endif

  CLOSE(client_fd);
  CLOSE(fd);

  printf("Server shutting down.\n");
  return 0;
}
