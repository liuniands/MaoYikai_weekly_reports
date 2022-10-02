#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
typedef struct Info
{
	int cfd;
	struct sockaddr_in cliaddr;
}INFO;

void *dealcli(void *arg)
{
	INFO *info = (INFO*)arg;
	char ip[16]="";
	printf("Client ip=%s, port=%d\n", 
			inet_ntop(AF_INET, &(info->cliaddr.sin_addr.s_addr), ip, 16), 
			ntohs(info->cliaddr.sin_port));
			
	while(1)
	{
		char buf[1024]="";
		int n = read(info->cfd, buf, sizeof(buf));
		if(n == 0)
		{
			printf("Client close\n");
			break;
		}
		printf("%s\n", buf);
		write(info->cfd, buf, n);
	}
	
	close(info->cfd);
	free(info); 
}

int main(int agrc, char *agrv[])
{

	int lfd = socket(AF_INET, SOCK_STREAM, 0);
	if(lfd < 0) perror("");

	struct sockaddr_in servaddr;
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(1037);
	servaddr.sin_addr.s_addr = 0;

	if(bind(lfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
		perror("");

	listen(lfd, 128);

	struct sockaddr_in cliaddr;
	socklen_t clilen = sizeof(cliaddr);
	pthread_t pid;
	while(1)
	{
		int cfd = accept(lfd, (struct sockaddr*)&cliaddr, &clilen);
		INFO *info = malloc(sizeof(INFO));
		info->cfd = cfd;
		info->cliaddr = cliaddr;
		pthread_create(&pid, NULL, dealcli, info);
		pthread_detach(pid);
	}
	
	close(lfd);
	return 0; 
} 
