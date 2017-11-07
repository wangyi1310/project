#include"ngx_pool.h"
#include<iostream>

#define ALIGN 8
#define ngx_align(p,a) (char *)(((long)p+((long)a-1)) & ~((long)a-1))
using namespace std;

NgxPool::NgxPool(size_t size):lbp(NULL)
{
//	head = (NgxPool *)new char[sizeof(NgxPool)];
	if(head == NULL)
	{
		throw("head failed");
	}
	sbp = (sblock *)new char[size];
	sbp->last = (char *)sbp+sizeof(sblock);
	sbp->end = (char *)sbp+size;
	sbp->next = NULL;
	sbp->failed = 0;
	current = sbp;
	lbp = NULL;
	max = (size < 255)? size : 255;//这里得加控制，
}

void *NgxPool::ngx_new(size_t size)
{
	sblock *tmp;
	char *m;
	if(size <= max)
	{
		tmp = current;
		do
		{
			m =  ngx_align(tmp->last,(ALIGN));
			if((size_t)(tmp->end-m) >= size)
			{
				tmp->last = m + size;
				return m;
			}
			tmp =tmp->next;
		}while(tmp);
		return ngx_new_sblock(size);
	}
	return ngx_new_lblock(size);
}

void * NgxPool::ngx_new_sblock(size_t size)
{
	char *m;
	size_t headsize;
	sblock *new_sblock;
	sblock *tmp;
	headsize = (size_t)((char *)sbp->end - (char *)sbp);
	m = new char[headsize];
	if(m == NULL)
	{
		return NULL;
	}
	new_sblock = (sblock *)m;

	new_sblock->end = m + headsize;
	new_sblock->next = NULL;
	new_sblock->failed = 0;

	m += sizeof(sblock);
	m = ngx_align(m,ALIGN);
	new_sblock->last = m+size;
	int sign =0;
	for(tmp = current; tmp->next != NULL ; tmp = tmp->next)
	{
		if(current->failed++ >= 4)
		{
			sign = 1;
			current = current->next;
			break;
		}
	}
	if(sign == 0)
	{
		current->failed = 0;
	}
	tmp->next = new_sblock;
	return m;
}

void NgxPool::reset_pool()
{
	sblock *sbptmp;
	for(sbptmp = sbp; sbptmp;sbptmp = sbptmp ->next)
	{
		sbptmp->last = (char *)sbptmp+sizeof(sblock);
		
	}
	lblock *lbpfirst = lbp;
	lblock *lbpsecond = lbpfirst;
	while(lbpfirst)
	{
		lbpsecond = lbpfirst->next;
		if(lbpfirst->p)
		{
			delete [](char *)lbpfirst->p;
		}
		delete []lbpfirst;
		lbpfirst = lbpsecond;
	}
	lbp = NULL;
}

void *NgxPool::ngx_new_lblock(size_t size)
{
	void *p;
	int n=0;
	lblock *lbptmp;
	p = new char[size];
	if(p == NULL)
	{
		return NULL;
	}
	for(lbptmp = lbp; lbptmp; lbptmp =lbptmp->next)
	{
		if(lbptmp->p == NULL)
		{
			lbptmp->p = p;
			return p;
		}
		n++;
		if(n > 3)
		{
			break;
		}
	}
	lbptmp =(lblock*) new char[sizeof(lblock)];
	if(lbptmp == NULL)
	{
		ngx_delete(p);
		return NULL;
	}
	lbptmp->p = p;
	lbptmp->next = lbp;
	lbp = lbptmp;
	return p;
}

void NgxPool::ngx_delete(void *p)
{
	lblock *lbptmp = lbp;
	while(lbptmp)
	{
		if(p == lbptmp->p)
		{
			delete [](char *)lbptmp->p;
			lbptmp->p = NULL;
		}
		lbptmp = lbptmp->next;
	}
}

NgxPool::~NgxPool()
{
	lblock *lbpfirst = lbp;
	lblock *lbpsecond = lbpfirst;
	sblock *sbpfirst = sbp;
	sblock *sbpsecond = sbpsecond;
	while(lbpfirst)
	{
		lbpsecond = lbpfirst->next;
		if(lbpfirst->p)
		{
			delete [](char *)lbpfirst->p;
		}
		delete []lbpfirst;
		lbpfirst = lbpsecond;
	}
	lbp = NULL;
	while(sbpfirst)
	{
		sbpsecond = sbpfirst->next;
		delete[]sbpfirst;
		sbpfirst =sbpsecond;
	}
	sbp = NULL;
	//delete []head;
}

