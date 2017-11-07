#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

int main()
{
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	assert(sockfd != -1);
	struct sockaddr_in saddr,caddr;
	memset(&saddr,0,sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(10000);
	saddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	int res = connect(sockfd,(struct sockaddr*)&saddr,sizeof(saddr));
	assert(res != -1);
	printf("input :\n");
	while(1)
	{
		char buff[128] = {0};
		fgets(buff,128,stdin);
		send(sockfd,buff,strlen(buff),0);
		recv(sockfd,buff,127,0);
		printf("%s\n",buff);
	}
	close(sockfd);
	return 0;
}
