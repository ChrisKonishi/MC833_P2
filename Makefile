all: client server

client:
	gcc client.c -Wall -o client

server:
	gcc server.c -Wall -o server

clean:
	rm -f client server