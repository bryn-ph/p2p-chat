#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
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

  // Read msg from client
  char buffer[1024];
  int RECIEVE_STATUS;

  while (1) {
    #ifdef _WIN32
      RECIEVE_STATUS = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    #else
      RECIEVE_STATUS = read(client_fd, buffer, sizeof(buffer) - 1);
    #endif

    if (RECIEVE_STATUS > 0) {
      buffer[RECIEVE_STATUS] = '\0';
      buffer[strcspn(buffer, "\r\n")] = '\0';
      printf("Received from client: %s\n", buffer);
    }
  }

  CLOSE(client_fd);
  CLOSE(fd);

  return 0;
}
