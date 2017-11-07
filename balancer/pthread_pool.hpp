#ifndef _POOL_PTHRAD_
#define _POOL_PTHRAD_
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>
//函数指针返回值是void* 参数是void×
typedef struct work
{
        Balancer *ba;//定义一个函数指针
        int arg;//指针的参数
       struct work *next;//指向下一个任务队列;
}CThred_worker;
typedef struct
{
    pthread_mutex_t queue_lock;//定义互斥锁
    pthread_cond_t  queue_ready;//定义条件变量一般配合互斥锁使用
    CThred_worker *queue_head;//定义任务队列
    int shutdown;//检测是不是要销毁线程：
    pthread_t *threadid;//指向每个线程数组的指针
    int max_thread_num;//设置线程的最大个数
    int cur_queue_size;//设置等待队列处理的线程数目
    CThred_worker *last;
}CThread_pool;
static CThread_pool *pool=NULL;
void *thread_routine(void *arg) //线程只能等待任务插入完成后才能触发去处理;
{ 
    while(1)
    {
        pthread_mutex_lock(&pool->queue_lock);
        while(pool->cur_queue_size==0&&!pool->shutdown)//阻塞的两个前当前队列为空并且不需要销毁线程池
        {
            pthread_cond_wait(&(pool->queue_ready),&(pool->queue_lock));//因为操做前会有一个解锁操作所以重新加
        }
        if(pool->shutdown)//判断当前线程池是不是需要关闭
        {
            pthread_mutex_unlock(&(pool->queue_lock));
             pthread_exit(NULL);
        }
        CThred_worker *work=pool->queue_head;
		assert(work!=NULL);
		pool->cur_queue_size--;
		pool->queue_head=work->next;
		if(pool->queue_head==NULL)
		{
			pool->last=NULL;
		}
        pthread_mutex_unlock(&(pool->queue_lock));//进行解锁
        (work->ba)->RecvSer(work->arg);//调用事件处理函数传入参数
       	free(work);
        work=NULL;
		
	}
}
int pool_destory()
{
    if(pool->shutdown)
    {
        return -1;
    }
    pool->shutdown=1;//防止多次调用destory
    pthread_cond_broadcast(&(pool->queue_ready));//唤醒所有线程
    int i=0;
    for(;i<pool->max_thread_num;i++)
    {
        pthread_join(pool->threadid[i],NULL);
    }
    free(pool->threadid);//先销毁线程在进行对任务队列的销毁
    CThred_worker *head=pool->queue_head;
    while(pool->queue_head!=NULL)
    {
        head=pool->queue_head->next;
        free(pool->queue_head);
        pool->queue_head=head;
    }
    //销毁条件变量和互斥锁
    pthread_mutex_destroy(&(pool->queue_lock));
    pthread_cond_destroy(&(pool->queue_ready));
    //销毁线程池
    free(pool);
    pool=NULL;
    return 0;
}
void pool_init(int max_thread_num) {
    pool = (CThread_pool *) malloc(sizeof(CThread_pool));
    pthread_mutex_init(&(pool->queue_lock), NULL);//初始化锁
    pthread_cond_init(&(pool->queue_ready), NULL);//初始化条件变量
    pool->queue_head = NULL;
    pool->max_thread_num = max_thread_num;
    pool->cur_queue_size = 0;
    pool->shutdown = 0;
    pool->last=NULL;
    pool->threadid = (pthread_t *) malloc(max_thread_num * sizeof(pthread_t));
    int i = 0;
    for (; i < max_thread_num; i++) {
        pthread_create(pool->threadid + i, NULL, thread_routine, NULL);
    }
}
int pool_add_work(Balancer *ba,int arg)
{
    CThred_worker *newworker=(CThred_worker*)malloc(sizeof(CThred_worker));//
	printf("new work add \n");
    CThred_worker **memeber=&pool->last;
    newworker->ba=ba;
    newworker->arg=arg;
    newworker->next=NULL;
	pthread_mutex_lock(&(pool->queue_lock));
	pool->cur_queue_size++;
    if(*memeber==NULL)
    {
        *memeber=newworker;
         pool->queue_head=newworker;//第一次创建新任务所以就直接给phead给值
    }
    else
    {
        (*memeber)->next=newworker;
    }
    pthread_mutex_unlock(&(pool->queue_lock));
    pthread_cond_signal(&(pool->queue_ready));//设置当前的信号被触发从线程池中可以取；添加完后才能取让这个线程获取感觉就是生产者和消费者的关系；
    return 0;
}
#endif
