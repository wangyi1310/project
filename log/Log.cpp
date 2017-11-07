#include "Log.h"
#include "mutex.h"
#include <stdlib.h>
Logger* Logger::log=NULL;
Mutex  Logger::mutex;
Logger* Logger::GetLog(const string &filename)
{
	if(log==NULL)
	{	
		mutex.lock();
		if(log==NULL)
		{
			log=new Logger(filename);
		}
		mutex.unlock();
	}
	return log;
}
Logger::Logger(const string &filename)
{
        fd=open(filename.c_str(),O_WRONLY|O_APPEND|O_CREAT,0666);
        if(fd==-1)
        {
                perror("Open Log File Error...");
                exit(0);
        }
}
Logger::~Logger()
{
	close(fd);
	free(log);
}
void Logger::LogNotice(const string &message)
{
        string prefix("[NOTICE:]");
        string log_time=get_current_time();
        mutex.lock();
        int erro=write(fd,prefix.c_str(),prefix.size());
        write(fd,message.c_str(),message.size());
        write(fd,log_time.c_str(),log_time.size());
        write(fd,"\n",2);
        mutex.unlock();
        printf("\033[32m%s%s %s\033[0m\n",prefix.c_str(),message.c_str(),log_time.c_str());
}
void Logger::LogWarn(const string &message)
{
        string prefix("[WARN:]");
        string log_time=get_current_time();
        mutex.lock();
        int erro=write(fd,prefix.c_str(),prefix.size());
        write(fd,message.c_str(),message.size());
        write(fd,log_time.c_str(),log_time.size());
        write(fd,"\n",2);
        mutex.unlock();
        printf("\033[33m%s%s %s\033[0m\n",prefix.c_str(),message.c_str(),log_time.c_str());
}
void Logger::LogError(const string &message)
{
        string prefix("[Error:]");
        string log_time=get_current_time();
        mutex.lock();
        int erro=write(fd,prefix.c_str(),prefix.size());
		//cout<<erro<<"*************************************************************************************************"<<endl;
        write(fd,message.c_str(),message.size());
        write(fd,log_time.c_str(),log_time.size());
        write(fd,"\n",2);
        mutex.unlock();
        printf("\033[31m%s%s %s\033[0m\n",prefix.c_str(),message.c_str(),log_time.c_str());
}
