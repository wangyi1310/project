#include <netinet/in.h>    // for sockaddr_in
#include <sys/types.h>    // for socket
#include <sys/socket.h>    // for socket
#include <stdio.h>        // for printf
#include <stdlib.h>        // for exit
#include <string.h>        // for bzero
#include <unistd.h>
#include <strings.h>


#define SERVER_PORT    6666
#define LENGTH_OF_LISTEN_QUEUE  20
#define BUFFER_SIZE 1024
#define REQUEST_MESSAGE_MAX_SIZE 512
#define SERVER_STRING "Server: luyaohttpd/0.1.0\r\n"

char method[255];
char file_name[255];
char request_message[REQUEST_MESSAGE_MAX_SIZE+1];
void headers(int client,int len)
{
 char buf[1024]="";
 bzero(buf, sizeof(buf));
 strcpy(buf, "HTTP/1.0 200 OK\r\n");
 send(client, buf, strlen(buf), 0);
 strcpy(buf, SERVER_STRING);
 send(client, buf, strlen(buf), 0);
 sprintf(buf, "Content-Type: text/html\r\n");
 send(client, buf, strlen(buf),0);
 sprintf(buf, "Content-Length: %d\r\n",len);
 send(client, buf, strlen(buf), 0);
 strcpy(buf, "\r\n");
 send(client, buf, strlen(buf), 0);
}




void not_found(int client)
{
 char buf[1024];
 bzero(buf, sizeof(buf));
 sprintf(buf, "HTTP/1.0 404 NOT FOUND\r\n");
 send(client, buf, strlen(buf), 0);
 sprintf(buf, SERVER_STRING);
 send(client, buf, strlen(buf), 0);
 sprintf(buf, "Content-Type: text/html\r\n");
 send(client, buf, strlen(buf), 0);
 sprintf(buf, "\r\n");
 send(client, buf, strlen(buf), 0);
 sprintf(buf, "<HTML><TITLE>Not Found</TITLE>\r\n");
 send(client, buf, strlen(buf), 0);
 sprintf(buf, "<BODY><P>The server could not fulfill\r\n");
 send(client, buf, strlen(buf), 0);
 sprintf(buf, "your request because the resource specified\r\n");
 send(client, buf, strlen(buf), 0);
 sprintf(buf, "is unavailable or nonexistent.\r\n");
 send(client, buf, strlen(buf), 0);
 sprintf(buf, "</BODY></HTML>\r\n");
 send(client, buf, strlen(buf), 0);
}

void bad_request(int client)
{
 char buf[1024];
 bzero(buf, sizeof(buf));
 sprintf(buf, "HTTP/1.0 400 BAD REQUEST\r\n");
 send(client, buf, sizeof(buf), 0);
 sprintf(buf, "Content-type: text/html\r\n");
 send(client, buf, sizeof(buf), 0);
 sprintf(buf, "\r\n");
 send(client, buf, sizeof(buf), 0);
 sprintf(buf, "<P>Your browser sent a bad request, ");
 send(client, buf, sizeof(buf), 0);
 sprintf(buf, "such as a POST without a Content-Length.\r\n");
 send(client, buf, sizeof(buf), 0);
}

void cannot_execute(int client)
{
 char buf[1024];
 bzero(buf, sizeof(buf));
 sprintf(buf, "HTTP/1.0 500 Internal Server Error\r\n");
 send(client, buf, strlen(buf), 0);
 sprintf(buf, "Content-type: text/html\r\n");
 send(client, buf, strlen(buf), 0);
 sprintf(buf, "\r\n");
 send(client, buf, strlen(buf), 0);
 sprintf(buf, "<P>Error prohibited CGI execution.\r\n");
 send(client, buf, strlen(buf), 0);
}
int get_line(char*request_message,int client){
  int i,j;
  i = 0; j = 0;
  while (!(request_message[j] ==' ') && (i < sizeof(method) - 1))
  {

   method[i] = request_message[j];
   i++; j++;
  }
  method[i] = '\0';
  i = 0;
  while (request_message[j] ==' ') j++;
  while (!(request_message[j] ==' ') && (i < sizeof(file_name) - 1) && (j < strlen(request_message)))
  {
   file_name[i] = request_message[j];
   i++; j++;
  }
  file_name[i] = '\0';
  if(strcasecmp(method,"GET") == 0)  return 2;
  else if(strcasecmp(method,"POST") == 0 ||strcasecmp(method,"PUT") == 0 ||
          strcasecmp(method,"DELETE") == 0 ||strcasecmp(method,"OPTIONS") == 0 ||
          strcasecmp(method,"TRACE") == 0 ||strcasecmp(method,"CONNETCT") == 0)     return 1;
  else return 0;
}
  
