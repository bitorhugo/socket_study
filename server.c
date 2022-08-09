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

	char welcome_msg [] = "Welcome!\n\0";
	int s, b, l, fd, sa, bytes, on = 1;
  struct sockaddr_in channel_srv; /* hold's IP address */
	struct pollfd poll_fds [MAX_CLIENTS];
  
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
	int num_clients = 0;

	int res = 0;

	/* Socket is now set up and bound. Wait for connection and process it. */
  while (1) {
		// timeout set to 0 -> poll won't block
		if ((res = poll(poll_fds, MAX_CLIENTS, 0) > 0)) {
			// check if new connection came in
			if (poll_fds[0].revents & POLLIN) {
				int client_fd = accept(s, 0, 0);
				//write welcome_msg to client
				write (client_fd, welcome_msg, strlen(welcome_msg));
				// check spot for new connection in poll_fd struct
				for (size_t i = 1; i < MAX_CLIENTS; i++) {
					if (poll_fds[i].fd == 0) { 
						poll_fds[i].fd = client_fd; // add new client to poll_fd stuct
						poll_fds[i].events = POLLIN;
						num_clients ++; // add to client counter
						break;
					}
				}
			}
		}

		for (size_t i = 1; i < MAX_CLIENTS + 1; i++) { // set an offset of 1 to num_clients due to poll_fds[0] -> server socket
			// check wether server can read without blocking from client i
			if (poll_fds[i].revents & POLLIN) {
				char buf [256];
				int bytes = read (poll_fds[i].fd, buf, sizeof(buf));
				write (poll_fds[i].fd, buf, strlen(buf));
				close (poll_fds[i].fd);
				poll_fds[i].fd = 0;
				poll_fds[i].events = 0;
				poll_fds[i].revents = 0;
				num_clients --;
			}
		}

  }

	return 0;
}
