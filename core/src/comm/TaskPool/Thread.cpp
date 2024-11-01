#include "Thread.h"

#include <pthread.h>

namespace LYW_PLUGIN_CORE
{
    bool Thread::CreateWorKThread(ThreadNode_t *threadNode)
    {
        pthread_t thread = {0};
        pthread_attr_t attr = {0};    

        if (NULL == threadNode)
        {
            return false;
        }

        //设置detach属性
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        if (pthread_create(&thread, &attr, WorkThreadEnter, threadNode) > 0 )
        {
            return false;
        }

        return true;
    }
}
