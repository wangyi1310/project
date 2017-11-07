#include "gettime.h"
string get_current_time()
{
        time_t timep;
        time(&timep);
        struct tm *p_tm=localtime(&timep);
        char timer[32]="";
        sprintf(timer,"[%d-%d %d:%d:%d]",1+p_tm->tm_mon,p_tm->tm_mday,p_tm->tm_hour,p_tm->tm_min,p_tm->tm_sec);
        return string(timer);
}
