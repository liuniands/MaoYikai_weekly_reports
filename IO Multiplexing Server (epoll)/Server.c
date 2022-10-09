#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/socket.h>
int main(int argc, char const *argv[])
{
	int lfd = socket(AF_INET, SOCK_STREAM, 0);
	if(lfd < 0)
		perror("");
	
	struct sockaddr_in servaddr;
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(1037);
	servaddr.sin_addr.s_addr = 0;

	if(bind(lfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
		perror("");
		
	listen(lfd,128);

	int epfd = epoll_create(1024);
	struct epoll_event tep, ep[1024];
	tep.events = EPOLLIN;
	tep.data.fd = lfd;
	epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &tep);
	
	struct sockaddr_in cliaddr;
	socklen_t len = sizeof(cliaddr);
	char ip[16]="";	
	while(1)
	{
		int n = epoll_wait(epfd, ep, 1024, -1);
		if(n < 0)
		{
			perror("");
			exit(1);
		}
		else if(n >= 0)
		{
			for(int i=0;i<n;i++)
			{
				int fd = ep[i].data.fd;
				if(fd == lfd && ep[i].events & EPOLLIN)
				{
					int cfd = accept(lfd, (struct sockaddr*)&cliaddr, &len);
					printf("Client ip=%s, port=%d\n",
						    inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, ip, 16),
						    ntohs(cliaddr.sin_port));

					tep.data.fd = cfd;
					tep.events = EPOLLIN;
					epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &tep);
				}
				else if(ep[i].events & EPOLLIN)
				{
					char buf[1024] = "";
					int readn = read(fd, buf, sizeof(buf));
					if(readn <= 0)
					{
						printf("Client close\n");
						close(fd);
						epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &ep[i]);
					}
					else
					{
						printf("%s\n", buf);
						write(fd, buf, readn);
					}
				}
			}
		}
	}
	return 0;
}
