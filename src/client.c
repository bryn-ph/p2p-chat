#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
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

int main(int argc, char *argv[]) {
   #ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
      printf("WSAStartup failed\n");
      exit(EXIT_FAILURE);
    }
  #endif

  SOCKET_TYPE fd;
  struct sockaddr_in addr;
  char *argAddr= argv[1];

  printf("Arg passed is: %s\n", argAddr);

  fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd == -1) {
    perror("Kappa Chungus this is not netWORKING");
    exit(EXIT_FAILURE);
  }

  memset(&addr, 0, sizeof(addr));  
  addr.sin_family = AF_INET;
  addr.sin_port = htons(8080);
  if (inet_pton(AF_INET, argAddr, &addr.sin_addr) <= 0) {
    perror("Invalid address");
    exit(EXIT_FAILURE);
  }

  // Connect to server
  printf("Trying to connect to server\n");
  socklen_t addrlen = sizeof(addr);
  if (connect(fd, (struct sockaddr *) &addr, addrlen) == -1) {
    perror("OOFT cannot connec");
    CLOSE(fd);
    exit(EXIT_FAILURE);
  }

  printf("Connected to %s on port 8080\n", argAddr);

  // Signal Handling
  // TODO: see if we can make windows interrupt fgets
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

  // Write data to server
  char buffer[1024];
  
  while (!stop) {
    printf("Enter message: ");
    if (!fgets(buffer, sizeof(buffer), stdin)) {
      if (stop || errno == EINTR) break;
      perror("fgets");
      break;
    }

    // Remove newline
    buffer[strcspn(buffer, "\r\n")] = '\0';

    #ifdef _WIN32
      int send_status = send(fd, buffer, strlen(buffer), 0) == -1;
    #else
      ssize_t send_status = write(fd, buffer, strlen(buffer)) == -1;
    #endif

    // Check if server is still connected
    char probe;
    ssize_t server_status = recv(fd, &probe, 1, MSG_PEEK | MSG_DONTWAIT);

    if (server_status == 0) {
        // Server closed the connection gracefully
        printf("Server disconnected (gracefully)\n");
        break;
    } else if (server_status < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
        // Error while probing socket
        perror("recv (checking socket state)");
        break;
    }

    if (send_status < 0) {
      perror("Could not write data");
    }
  }

  CLOSE(fd);

  printf("Client shutting down!\n");
  return 0;
}

