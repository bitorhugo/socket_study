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
#define PORT 8080
#define BUF_SZ 4096

int main (int argc, char** argv) {

	int s, b, l, fd, sa, bytes, on = 1;
  char buf[BUF_SZ]; /* content read */
  struct sockaddr_in channel_srv;
  
  

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
