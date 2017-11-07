#ifndef _LOG_
#define _LOG_
#include "mutex.h"
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include "gettime.h"
using namespace std;
class Logger
{
       private:
        	    Logger(const string &filename);
                bool LOGFILE;
                int fd;
                static Logger* log;
                static Mutex mutex;
        public:
				static Logger *GetLog(const string &filename);
                void LogNotice(const string &str);
                void LogError(const string &str);
                void LogWarn(const string &str);
				~Logger();
};
#endif
