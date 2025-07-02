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
    printf("Kappa Chungus this is not netWORKING\n");
    exit(EXIT_FAILURE);
  }

  memset(&addr, 0, sizeof(addr));  
  addr.sin_family = AF_INET;
  addr.sin_port = htons(8080);
  if (inet_pton(AF_INET, argAddr, &addr.sin_addr) <= 0) {
    printf("Invalid address\n");
    exit(EXIT_FAILURE);
  }

  // Connect to server
  printf("Trying to connect to server\n");
  socklen_t addrlen = sizeof(addr);
  if (connect(fd, (struct sockaddr *) &addr, addrlen) == -1) {
    printf("OOFT cannot connect\n");
    CLOSE(fd);
    exit(EXIT_FAILURE);
  }

  printf("Connected to %s on port 8080\n", argAddr);

  // Write data to server
  char buffer[1024];
  
  while (1) {
    printf("Enter message: ");
    if (!fgets(buffer, sizeof(buffer), stdin)) {
      printf("Wadafuq fgets error!?!");
      break;
    }

    // Remove newline
    buffer[strcspn(buffer, "\r\n")] = '\0';

    #ifdef _WIN32
      int SEND_STATUS = send(fd, buffer, strlen(buffer), 0) == -1;
    #else
      int SEND_STATUS = write(fd, buffer, strlen(buffer)) == -1;
    #endif
    if (SEND_STATUS) {
      printf("Could not write data\n");
    }
    printf("Sending msg\n");
  }

  CLOSE(fd);

  return 0;
}

