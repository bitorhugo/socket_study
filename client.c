#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define SERVER_PORT 10010  
#define BUF_SIZE 4096 /* block transfer size */

int main(int argc, char **argv) {

  int c, s, bytes;
  char buf[BUF_SIZE]; /* buffer for incoming file */
  struct hostent *h; /* info about server */
  struct sockaddr_in channel; /* holds IP address */
  
  if (argc != 3) {
      printf("Usage: is-client server-name message-to-deliever\n");
      exit(EXIT_FAILURE);
  }

  h = gethostbyname(argv[1]); /* look up host's IP address */
  if (!h) {
      perror("gethostbyname");
      exit(EXIT_FAILURE);
  }
  
  s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (s < 0) {
      perror("socket");
      exit(EXIT_FAILURE);
  }
  
  memset(&channel, 0, sizeof(channel));
  channel.sin_family = AF_INET;
  memcpy(&channel.sin_addr.s_addr, h->h_addr, h->h_length);
  channel.sin_port= htons(SERVER_PORT);

  c = connect(s, (struct sockaddr *) &channel, sizeof(channel));
  if (c < 0) {
      perror("connect");
      exit(EXIT_FAILURE);
  }
  
  
  /* Connection is now established. Send message plus \0 byte at the end */
  write(s, argv[2], strlen(argv[2])+1);
  
  while (1) {
      bytes = read(s, buf, BUF_SIZE); /* read from socket */
      if (bytes <= 0) exit(EXIT_SUCCESS); /* check for end of file */
      write(1, buf, bytes); /* write to standard output */
  }
}
