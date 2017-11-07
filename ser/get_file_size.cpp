#include "get_file_size.h"
#include <sys/stat.h>
unsigned long get_file_size(const char *filename)  
{  
   struct stat buf;  
   if(stat(filename, &buf)<0)  
   {  
       return 0;  
   }  
 	 return (unsigned long)buf.st_size;  
}  
