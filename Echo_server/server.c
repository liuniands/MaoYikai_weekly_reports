#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 1037

int main(int argc, char *argv[])
{
	int lfd = socket(AF_INET, SOCK_STREAM, 0);
	if(lfd < 0)
		perror("");

	struct sockaddr_in servaddr;
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = 0;
	
	bind(lfd, (struct sockaddr*)&servaddr, sizeof(servaddr));

	listen(lfd, 128);
	
	struct sockaddr_in cliaddr;
	socklen_t len = sizeof(cliaddr);
	int cfd = accept(lfd, (struct sockaddr *)&cliaddr, &len);
	if(cfd < 0 )
		perror("");
		
	char ip[16]="";
	printf("Client ip=%s, port=%d\n",
			inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, ip, 16),
			ntohs(cliaddr.sin_port));

	while(1)
	{
		char buf[256]="";
		int n = read(cfd, buf, sizeof(buf));
		if(n == 0)
		{
			printf("Client close\n");
			break;
		}
		printf("%s", buf);
		write(cfd, buf, n);
	}

	close(lfd);
	close(cfd);
	return 0;
}

