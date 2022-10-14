#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

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

	if(bind(lfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
		perror("");
	
	listen(lfd, 128);
	
	int maxfd = lfd;
	fd_set rset;
	fd_set allset;
	FD_ZERO(&rset);
	FD_ZERO(&allset);
	FD_SET(lfd, &allset);
	
	int n = 0;
	struct sockaddr_in cliaddr;
	socklen_t clilen = sizeof(cliaddr);
	char ip[16] = "";
	char buf[1024] = "";
	while(1)
	{
		rset = allset;
		n = select(maxfd+1, &rset, NULL, NULL, NULL);
		if(n < 0)
		{		
			perror("");
			break;
		}
		else if(n >= 0)
		{

			if(FD_ISSET(lfd, &rset))
			{
				int cfd = accept(lfd, (struct sockaddr*)&cliaddr, &clilen);
				printf("Client ip=%s, port=%d\n",
						inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, ip, 16),
						ntohs(cliaddr.sin_port));
			
				FD_SET(cfd, &allset);
			
				if(maxfd < cfd)
					maxfd = cfd;
					
				if(--n == 0)
					continue;
			}
			
			for(int i=lfd+1;i<=maxfd;i++)
			{
				if(FD_ISSET(i, &rset))
				{
					int n = read(i, buf, sizeof(buf));
					if(n == 0)
					{
						printf("Client close\n");
						close(i);
						FD_CLR(i,&allset);
						
						if(i != maxfd)
						{
							dup2(maxfd, i);
							close(maxfd);
							maxfd--;
						}
					}
					else if(n < 0)
					{
						perror("");
					}
					else
					{
						printf("%s\n",buf);
						write(i, buf, n);
					}
				}
				if(--n == 0)
					break; 
			}
		}
	}
	
	close(lfd);
	return 0;
}
