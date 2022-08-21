#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>
#include <stdlib.h>
#include <stdio.h>

/**
 * In this example the call to read is blocking
 * In order for write to STDOUT to happen,
 * we need to deliver an EOF to STDIN e.g: <CR> | ^D
 */
void blocking (void) {
	char buf [256];
	read(STDIN_FILENO, buf, sizeof(buf));
	write (STDOUT_FILENO, buf, strlen(buf));
}

/*
 * In the non_blocking version we develop an event loop
 * This event loop will poll the set of fds that we want to handle
 * in this case we are waiting for STDIN to be available to read from it
 * When the fd is available, the poll function will warn us that it is capable of being handled
 */
void non_blocking (void) {
	char buf [256];
	int count = 0;
	int res;
	struct pollfd pollfd;
	pollfd.fd = STDIN_FILENO; // we want to look out for events happening to stdin
	pollfd.events = POLLIN; // define what type of events we want to look out for
	
	// start the event loop
	while (1) {

		if ((res = poll(&pollfd, 1, 0)) < 0) { // set timeout to 0 so poll won't block
			perror ("Usage: poll");
			break;
		}
		else if (res > 0) { // fd is available for i/o (in this case in STDIN ready for READING)
			read (STDIN_FILENO, buf, sizeof(buf));	
			break;
		}
		else {
			count ++;
		}
	}

	printf ("-- %s\n", buf);
	printf ("time elapsed: %d ms\n", count);

}

int main (void) {

	//blocking();
	non_blocking();

	return 0;
}
