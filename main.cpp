#include "balancer/Balancer.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <error.h>
#include <fcntl.h>
int main()
{
	Balancer ba("10.104.238.179",8000);
	ba.Run();
	return 0;
}
