all: server.c
	cc server.c -o bin/server
	cc client.c -o bin/client
