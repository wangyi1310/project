#include<iostream>
#include"ngx_pool.h"
int main()
{
	NgxPool np1(255);
	char *p = (char *)np1.ngx_new(10);
	np1.ngx_delete(p);

}
