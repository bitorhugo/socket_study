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

#define QUEUE_SZ 5
#define BUF_SZ 4096

int main (int argc, char** argv) {

	printf("");
	/* create port numbers */
	int np = 5, base_port = 8080;
	int ports [np];
	for (size_t i = 0; i < np; i++) {
		ports[i] = base_port + i;
	}

	int s[np], b[np], l[np], fd[np], sa[np], bytes, on = 1;
  char buf[BUF_SZ]; /* content read */
  struct sockaddr_in channel_srv [np];
  
  /* Build address structure to bind to socket. */
	for (size_t i = 0; i < np; i++) {
		memset(&channel_srv[i], 0, sizeof(channel_srv)); /* zero channel */
  	channel_srv[i].sin_family = AF_INET;
  	channel_srv[i].sin_addr.s_addr = htonl(INADDR_ANY);
  	channel_srv[i].sin_port = htons(ports[i]);

	  /* Passive open. Wait for connection. */
	  s[i] = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); /* create socket */
	  if (s[i] < 0) {
	      perror("Usage: socket");
	      exit(EXIT_FAILURE);
	  }
		printf("Socket Created\n");
	  
	  b[i] = bind(s[i], (struct sockaddr *) &channel_srv[i], sizeof(channel_srv));
	  if (b[i] < 0) {
	      perror("Usage: bind");
	      exit(EXIT_FAILURE);
	  }
		printf("Bind done\n");
	
	  l[i] = listen(s[i], QUEUE_SZ); /* specify queue size */
	  if (l[i] < 0) {
	      perror("Usage: listen");
	      exit(EXIT_FAILURE);
	  }
		printf("Listening..\n");
	}
  

	/* Socket is now set up and bound. Wait for connection and process it. */
  while (1) {
		fd_set read_fds;
		FD_ZERO(&read_fds);

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
