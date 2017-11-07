#ifndef _NGX_POOL_
#define _NGX_POOL_
#include<iostream>
typedef struct _small_block //小的数据块
{
	char *last;
	char *end;
	struct _small_block *next;
	int failed;
}sblock;
typedef struct _large_block//大的数据块
{
	void *p;
	struct _large_block *next;
}lblock;
class NgxPool//内存池模块
{
public:
	NgxPool(size_t size);
	void *ngx_new(size_t size);
	void ngx_delete(void *p);
	void reset_pool();
	~NgxPool();
//private:
	void *ngx_new_sblock(size_t size);
	void *ngx_new_lblock(size_t size);
	sblock  *sbp;
	NgxPool *head;
	lblock *lbp;
	size_t max;
	sblock *current;
};
#endif
