#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
int main(int argc, char *argv[])
{
	int cfd = socket(AF_INET, SOCK_STREAM, 0);
	if(cfd < 0)
		perror("");
		
	int port;
	char ip[16] = "";
	printf("Please enter server's ip and port\n");
	scanf("%s%d", ip, &port);
	
	struct sockaddr_in servaddr;
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	servaddr.sin_addr.s_addr = inet_addr(ip);
	
	if(connect(cfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
		perror("");

	while(1)
	{
		char buf[1024]="";
		char rebuf[1024]="";
		fgets(buf, sizeof(buf), stdin);
		buf[strlen(buf) - 1] = 0;
		write(cfd, buf, strlen(buf));
		
		int n = read(cfd, rebuf, sizeof(rebuf));
		if(n == 0)
		{
			printf("Server close\n");
			break;
		}
		printf("%s\n", rebuf);
	}

	close(cfd);
	return 0;
}

