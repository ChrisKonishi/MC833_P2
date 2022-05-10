all: client server

client:
	gcc client.c -Wall -O3 -o client

server:
	gcc server.c -Wall -O3 -o server

clean:
	rm -f client server