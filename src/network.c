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
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(8080);

  if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
    printf("Kappa Chungus this is not netWORKING BIND\n");
    close(fd);
    exit(EXIT_FAILURE);
  }

  // Listen for incoming connections
  if (listen(fd, 5) == -1) {
    printf("OOFT cannot listen\n");
    close(fd);
    exit(EXIT_FAILURE);
  }

  printf("Listening on port 8080\n");

  struct sockaddr_in client_addr;
  socklen_t client_len = sizeof(client_addr);
  int client_fd = accept(fd, (struct sockaddr *)&client_addr, &client_len);
  if (client_fd == -1) {
    printf("shocka client_fd fail\n");
    close(fd);
    exit(EXIT_FAILURE);
  }

  printf("Client connected!\n");

  close(client_fd);
  close(fd);

  return 0;

  // inet_pton(AF_INET, "202.14.192.86", &server_addr.sin_addr);
  // server_addr.sin_family = AF_INET;
  // server_addr.sin_port = htons(80);
}
