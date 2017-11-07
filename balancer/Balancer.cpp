#include "Balancer.h"
#include "json/json.h"
#include <stdlib.h>
#include <sys/epoll.h>
#include <pthread.h>
#include <string>
#include <string.h>
#include "../log/Log.h"
#include "httpheader.hpp"
#include "pthread_pool.hpp"
#include "../memory/ngx_pool.h"
#include<sys/sendfile.h>
#define MAXFD 1024
#define PATH "public"
int Balancer::count=0;
int setnonblocking(int fd)
{
	int old_option=fcntl(fd,F_GETFL);
	int new_option=old_option | O_NONBLOCK;
	fcntl(fd,F_SETFL,new_option);
	return old_option;
}
void addfd(int epollfd,int fd)
{
	epoll_event event;
	event.data.fd=fd;
	event.events=EPOLLIN | EPOLLET;
	epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&event);
	setnonblocking(fd);
}
void delfd(int epollfd,int fd)
{
	epoll_ctl(epollfd,EPOLL_CTL_DEL,fd,NULL);
}
void* worker(void *arg)
{
		Balancer *ba=static_cast<Balancer*>(arg);
		epoll_event events[MAXFD];
		int epollfd=epoll_create(1024);
		if(epollfd==-1)
		{
			ba->log->LogWarn("pthread create fail");
			return NULL;
		}
		ba->log->LogNotice("pthread create success");
		addfd(epollfd,ba->mListenfd);
		int ret=0;
		int sockfd=0;
		while(1)
		{
			ret=epoll_wait(epollfd,events,1024,-1);
			if(ret<0)
			{
				break;
			}
			for(int i=0;i<ret;i++)
			{
				sockfd=events[i].data.fd;
				if(sockfd==ba->mListenfd)
				{
					int connetfd=ba->Accept();
					if(connetfd>=0)
					{
						addfd(epollfd,connetfd);
					}
				}
				else
				{
					
					int i=ba->RecvCli(sockfd);
					if(i==-1)
					{
						delfd(epollfd,sockfd);
						close(sockfd);						
					}
				}
			}

		}

}
void Balancer::Run()
{
	pthread_t pid;
	epoll_event events[MAXFD];
	int epollfd=epoll_create(1024);
	assert(epollfd!=-1);
	for(int i=0;i<vec.size();i++)
	{
		addfd(epollfd,vec[i]);
	}
	for(int i=0;i<3;i++)
	{
		pthread_create(&pid,NULL,worker,(void*)this);
	}
	char buff[1024]="";
	pool_init(3);
	while(1)
	{
		int ret=epoll_wait(epollfd,events,1024,-1);
		for(int i=0;i<ret;i++)
		{
			int socketfd=events[i].data.fd;
			pool_add_work(this,socketfd);
		}
	}
}


Balancer::Balancer(const char * ip,unsigned short port)
{
	mListenfd=socket(AF_INET,SOCK_STREAM,0);
	log=Logger::GetLog(string("SeversLog.txt"));
	if(mListenfd==-1)
	{
		log->LogError(string("Create mListenfd is fail"));
		exit(0);
	}
	sockaddr_in saddr,caddr;
	memset(&saddr,0,sizeof(saddr));
	saddr.sin_family=AF_INET;
	saddr.sin_port=htons(port);
	saddr.sin_addr.s_addr=inet_addr(ip);
	int res=bind(mListenfd,(struct sockaddr*)&saddr,sizeof(saddr));
	if(res==-1)
	{
		log->LogError(string("Bind mListen is fail"));
		exit(0);
	}
	listen(mListenfd,5);
	log->LogNotice("Server Listen Start is Ok ");
	if(LoadEtcSer("etc/etc.txt")!=true&&mSerMap.size()==0)
	{
		exit(0);
	}
	InitServers();
	if(mConser.size()<1)
	{
		exit(0);
	}
}
int Balancer::Accept()
{
	sockaddr caddr;
	socklen_t len=sizeof(caddr);
	return accept(mListenfd,(sockaddr*)&caddr,&len);
}
int Balancer::RecvCli(int c)
{
	log->LogWarn("send server ing");
	Json::Value root;
	root["cfd"]=c;
	char buff[256]="";
	
	int i=recv(c,buff,256,0);
	
	if(i<=0)
	{
		return -1;
	}

	root["data"]=buff;
	
	string str=root.toStyledString();
	cout<<str;
	char arr[10]="";
	sprintf(arr,"%d",c);

	HaNode* sfd=hc.GetServer(arr);
	send(sfd->mfd,str.c_str(),strlen(str.c_str()),0);//客户端以来就将他转发；防止惊群现象采用锁
	log->LogWarn("send sever compelete");
	return 0;
}
bool Balancer::LoadEtcSer(const char *etcpath)
{
	int fd=open(etcpath,O_RDONLY);
	if(fd==-1)
	{
		log->LogError(string("Open etcfile is fail"));
		return false;
	}
	log->LogNotice("Open Etcfile is Ok");
	Json::Reader reader;
	char etc[256]="";
	int i=read(fd,etc,256);
	
	Json::Value root;
	reader.parse(etc,root);
	int sers=root["num"].asInt();
	unsigned  port;
	char serinfo[20]="";
	for(int i=1;i<sers+1;i++)
	{
		
		sprintf(serinfo,"%dip",i);
		string str=root[serinfo].asString();
		sprintf(serinfo,"%dport",i);
		port=root[serinfo].asInt();
		mSerMap.insert(make_pair(str,port));
		
	}
	log->LogNotice("etc ok");
	return true;
}
void Balancer::InitServers()
{
	int Confd;
	map<string,unsigned short>::iterator it=mSerMap.begin();
	char buff[128]="";
	for(;it!=mSerMap.end();it++)
	{
		Confd=socket(AF_INET,SOCK_STREAM,0);
		sockaddr_in caddr;
		memset(&caddr,0,sizeof(caddr));
		caddr.sin_family=AF_INET;
		caddr.sin_port=htons(it->second);
		caddr.sin_addr.s_addr=inet_addr((it->first).c_str());
		int res=connect(Confd,(sockaddr*)&caddr,sizeof(caddr));
		strcpy(buff,(it->first).c_str());
		sprintf(buff+strlen(buff)," port=%d",it->second);
		if(res==-1)
		{
			strcat(buff,":Server COnnect fail");
			log->LogError(buff);
		}
		else
		{
			strcat(buff,":Server Connect success");
			mConser.insert(make_pair(it->first,Confd));
			string str=it->first;
			str+=it->second;
			hc.AddHaNode(Confd,str);
			vec.push_back(Confd);
			log->LogNotice(buff);
		}
		memset(buff,0,128);
	}
}
void Balancer::RecvSer(int fd)
{
	Json::Reader reader;
	char buff[128]="";
	Json::Value root;
	recv(fd,buff,4,0);
	int jlen=atoi(buff);
	memset(buff,0,4);
	recv(fd,buff,jlen-1000,0);
	cout<<"	"<<buff<<endl;
	reader.parse(buff,root);
	int clifd=root["cfd"].asInt();
	int cmd=root["cmd"].asInt();
	if(cmd==0)
	{
		bad_request(clifd);
		return ;
	}	
	int state=root["state"].asInt();
	if(state==0)
	{
		not_found(clifd);	
		return ;
	}
	int len=root["len"].asInt();
	headers(clifd,len);
	string tmp=root["path"].asString();
	char path[128]={PATH};
	strcat(path,tmp.c_str());
	int fr=open(path,O_RDONLY);
	if(fr==-1)
	{
		log->LogError("open file errro");
		strcat(path,"open fial");
		log->LogError("path");
		close(fr);	
		return ;
	}
	char data[1024]="";
	int count=0;
	log->LogNotice("sending data");
	sendfile(clifd,fr,NULL,len);
	log->LogNotice("send complete");
	close(fr);
}
