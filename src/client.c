#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
  int fd;
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
  // addr.sin_port = htons(8080);
  if (inet_pton(AF_INET, argAddr, &addr.sin_addr) <= 0) {
    printf("Invalid address\n");
    exit(EXIT_FAILURE);
  }

  // Connect to server
  printf("Trying to connect to server\n");
  socklen_t addrlen = sizeof(addr);
  if (connect(fd, (struct sockaddr *) &addr, addrlen)) {
    printf("OOFT cannot connect\n");
    close(fd);
    exit(EXIT_FAILURE);
  }

  printf("Connected to %s on port 8080\n", argAddr);

  // Write data to server
  char data[] = "Hello world!";
  int datalen = strlen(data);

  if (write(fd, data, datalen) == -1) {
    printf("Could not write data\n");
  }

  close(fd);

  return 0;
}

