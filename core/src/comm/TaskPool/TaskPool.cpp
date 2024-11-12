#include "TaskPool.h"
#include "Log/Log.h"


#include <sys/prctl.h>
#include <stdlib.h>
#include <unistd.h>

namespace LYW_PLUGIN_CORE
{
    TaskPool::TaskPool(uint32 taskCount, uint32 holdThread, uint32 maxThread) : Thread(holdThread, maxThread)
    {
        m_isRun = true;
    }

    TaskPool::~TaskPool()
    {
        m_isRun = false;
        
        //等待任务结束 超时时间内任务未终止 则报错 可能导致崩溃
    }

    TaskHandle TaskPool::AsyncExc(ID id, TaskFunc func, pvoid usrPtr, const byte describe[128])
    {
        return -1;
    }

    eTaskState TaskPool::Wait(TaskHandle taskHandle, int32 timeout)
    {
        return TASK_WAIT_TIMEOUT;
    }

    eTaskState TaskPool::WaitByID(ID id, int32 timeout)
    {
        return TASK_WAIT_TIMEOUT;
    }


    bool TaskPool::Remove(TaskHandle taskHandle, TaskFunc &recycleFunc)
    {
        return TASK_WAIT_TIMEOUT;
    }

    bool TaskPool::RemoveByID(ID id, TaskFunc &recycleFunc)
    {
        return TASK_WAIT_TIMEOUT;
    }

    pvoid TaskPool::WaitTask(int32 timeout)
    {
        return NULL;
    }

    void TaskPool::ExcuteTask(pvoid taskNode)
    {

    }

    pvoid TaskPool::Daemon(pvoid *ptr)
    {
        TaskPool * self = (TaskPool *)ptr;
        prctl(PR_SET_NAME, "TaskPool_Work_Daemon", 0, 0, 0);

        if (NULL != self)
        {
            self->DaemonRun();
        }
        else
        {
            LOG_ERROR("TaskPool Ptr Nil! Program Exit!!!");
            exit(-1);
        }
        return NULL;
    }

    void TaskPool::DaemonRun()
    {
        while (m_isRun)
        {
            Thread::Daemon();
            usleep(10000);
        }
    }

    int32 TaskPool::TaskResourceAssessment()
    {
        return 0;
    }
}
