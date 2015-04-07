#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>

#include "const.h"

int main(int argc, char *argv[])
{
	if (argc != 3) {
		printf("error: too few or too many parameters\n");
		printf("usage: %s SERVER_IP PATH_TO_FILE\n", argv[0]);
		return -1;
	}
	int sock;
	struct sockaddr_in addr;
	struct hostent *server;
	char answer[SIZE], ans;
	char buf[BUF_SIZE];
	int i, bytesrecvd = 0;

	sock = socket(AF_INET, SOCK_STREAM, 0);

	server = gethostbyname(argv[1]);
	if (server == NULL) {
		printf("error: host %s does not exists\n", argv[1]);
		return -1;
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(3425);
	bcopy((char *)server->h_addr, (char *)&addr.sin_addr.s_addr, server->h_length);

	if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        	printf("error: unable to connect\n");
		return -1;
	}

	send(sock, argv[2], strlen(argv[2])+1, 0);
	recv(sock, answer, SIZE, 0);
	printf(answer);
	answer[5]='\0';
	if (!strcmp(answer, "error")) {
		close(sock);
		return -1;
	}

	scanf("%c", &ans);
	if ((ans != 'y') && (ans != 'Y')) {
		ans = 'n';
		send(sock, &ans, 1, 0);
		close(sock);
		return 0;
	}

	i = strlen(argv[2])-1;
	while ((i > 0) && (argv[2][i] != '/'))
		i--;
	if (i)
		strcpy(argv[2], argv[2]+i+1);

	FILE *fout = fopen(argv[2], "wb");
	if (!fout) {
		printf("error: can't open file %s for writing\n", argv[2]);
		ans = 'n';
		send(sock, &ans, 1, 0);
		close(sock);
		return -1;
	}

	send(sock, &ans, 1, 0);
	bytesrecvd = recv(sock, buf, BUF_SIZE, 0);
	while (bytesrecvd > 0) {
		fwrite(buf, 1, bytesrecvd, fout);
		bytesrecvd = recv(sock, buf, BUF_SIZE, 0);
	}
	close(sock);
	fclose(fout);
	return 0;
}
