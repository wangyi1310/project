#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <json/json.h>
#include "get_file_size.h"
#define PATH "../public"
using namespace std;
void senda(int fd,Json::Value &root)
{
	string str=root.toStyledString();
	int len=strlen(str.c_str());
	char buff[128]="";
	sprintf(buff,"%d",1000+len);
	strcat(buff,str.c_str());
	send(fd,buff,strlen(buff),0);

}
int main()
{
	int sockfd=socket(AF_INET,SOCK_STREAM,0);
	assert(sockfd!=-1);
	struct sockaddr_in saddr,caddr;
	memset(&saddr,0,sizeof(saddr));
	saddr.sin_family=AF_INET;
	saddr.sin_port=htons(6000);
	saddr.sin_addr.s_addr=inet_addr("127.0.0.1");
	int res=bind(sockfd,(struct sockaddr *)&saddr,sizeof(saddr));
	assert(res!=1);
	listen(sockfd,5);
	Json::Value root,root1;
	Json::Reader reader;
	int i=0;
		socklen_t len=sizeof(caddr);
		int c=accept(sockfd,(struct sockaddr*)&caddr,&len);
		if(c<0)
		{
				return 0;	
		}
		cout<<"fdsfdsfas"<<endl;
		char buff[512]={0};
		while(1)
		{
			int i=recv(c,buff,511,0);
		if(i<=0)
		{
			break;	
		}
		reader.parse(buff,root);
		int clifd=root["cfd"].asInt();
		string str=root["data"].asString();
		strcpy(buff,str.c_str());
		printf("%s\n",buff);
		char *s=strtok(buff," ");
		root1["cfd"]=clifd;
		if(strcmp(s,"GET")!=0)
		{
			root1["cmd"]=0;
			senda(clifd,root1);
			continue;
		}

		s=strtok(NULL," ");
		if(s==NULL)
		{
			root1["cmd"]=0;
			senda(c,root1);
			continue;
		}
		root1["cmd"]=1;
		char f_path[128]={PATH};
		strcat(f_path,s);
		root1["path"]=s;
		len=get_file_size(f_path);
		root1["state"]=1;
		root1["len"]=len;
		senda(c,root1);
	}
}
