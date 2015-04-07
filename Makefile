all:
	gcc -pthread serv.c -o server
	gcc client.c -o getfile

server:
	gcc -pthread serv.c -o server

client:
	gcc client.c -o client

clean:
	rm -f serv client
