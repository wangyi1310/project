#include <stdio.h>
#define ngx_align(p,a) \
	(char *)(((unsigned int)p->last+((unsigned int)a-1)) & ~((unsigned int)a-1))
int main()
{
	char *p = 10;
	printf("%p\n", ngx_align(p, 8));
	return 0;
}
