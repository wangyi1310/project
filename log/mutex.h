#ifndef _MUTEX_
#define _MUTEX_
#include <pthread.h>
#include <assert.h>
class Mutex
{
        private:
                int error;
                pthread_mutex_t mutex;
        public:
        Mutex();
        void lock();
        void unlock();
        ~Mutex();
};
#endif
