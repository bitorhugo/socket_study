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
#include <sys/poll.h>

#define QUEUE_SZ 5
#define MAX_CLIENTS 10
#define PORT 10010
#define BUF_SZ 4096

int main (int argc, char** argv) {

	int s, b, l, fd, sa, bytes, on = 1;
  char buf[BUF_SZ]; /* content read */
  struct sockaddr_in channel_srv; /* hold's IP address */
	struct pollfd poll_fds [MAX_CLIENTS + 1];
  
  /* Build address structure to bind to socket. */
  memset(&channel_srv, 0, sizeof(channel_srv)); /* zero channel */
  channel_srv.sin_family = AF_INET;
  channel_srv.sin_addr.s_addr = htonl(INADDR_ANY);
  channel_srv.sin_port = htons(PORT);
  
  /* Passive open. Wait for connection. */
  s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); /* create socket */
  if (s < 0) {
      perror("Usage: socket");
      exit(EXIT_FAILURE);
  }
	printf("Socket Created\n");

	/* set socket options as reusable */
	if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on)) < 0) {
		perror("Usage: setsockopt");
	}
  
  b = bind(s, (struct sockaddr *) &channel_srv, sizeof(channel_srv));
  if (b < 0) {
      perror("Usage: bind");
      exit(EXIT_FAILURE);
  }
	printf("Bind done\n");

  l = listen(s, QUEUE_SZ); /* specify queue size */
  if (l < 0) {
      perror("Usage: listen");
      exit(EXIT_FAILURE);
  }
	printf("Listening..\n");
 

	memset(&poll_fds, 0, sizeof(poll_fds));
	poll_fds[0].fd = s;
	poll_fds[0].events = POLLIN;
	int in_use_client = 0;


	/* Socket is now set up and bound. Wait for connection and process it. */
  while (1) {

		// check to see if any fd is ready 
		int num_ready = poll(poll_fds, in_use_client + 1, 10000);
		if (num_ready < 0) {
			perror("Usage: poll");
			exit(EXIT_FAILURE);
		} 

    sa = accept(s, 0, 0); /* block for connection request */
    if (sa < 0) {
        perror("Accept");
        continue;
    }
		printf("Conn accepted\n");

		  
		/* reads content received from client */
    if((bytes = read(sa, buf, BUF_SZ)) <= 0){
        close(sa);
    }
		printf("Message received\n");

		/* write message to socket */
		write(sa, buf, strlen(buf));
		printf("3-Message sent\n");

    close(sa);  /* close connection */
		printf("4-Conn closed\n");
  }

	return 0;
}
