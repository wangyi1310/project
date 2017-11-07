#include "mutex.h"
Mutex::Mutex()
{
        error=pthread_mutex_init(&mutex,NULL);
        assert(error==0);
}
void Mutex::lock()
{
        error=pthread_mutex_lock(&mutex);
}
void Mutex::unlock()
{
        error=pthread_mutex_unlock(&mutex);
}
Mutex::~Mutex()
{
        error=pthread_mutex_destroy(&mutex);
}

