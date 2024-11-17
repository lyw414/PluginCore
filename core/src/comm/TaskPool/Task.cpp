#include "Task.h"

#include <string>
#include <string.h>
#include <stdlib.h>
#include <time.h>

namespace LYW_PLUGIN_CORE
{
    Task::Task(uint32 maxTaskCount)
    {
        srand(time(NULL));
        m_maxTaskCount = maxTaskCount;

        m_nodeArray = new(std::nothrow) NodeList::Node<TaskNode_t>[m_maxTaskCount];

        //添加至空闲队列 
        for (uint32 iLoop = 0; iLoop < m_maxTaskCount; iLoop++)
        {
            m_nodeArray[iLoop].data.index = iLoop;

            m_nodeArray[iLoop].data.uuid = rand();

            m_freeNodeList.PushBack(&m_nodeArray[iLoop]);
        }

        pthread_mutex_init(&m_lock, NULL);

        pthread_mutex_init(&m_condLock, NULL);

        pthread_cond_init(&m_cond, NULL);
    }

    Task::~Task()
    {
        if (NULL != m_nodeArray)
        {
            delete [] m_nodeArray;
            m_nodeArray = NULL;
        }
    }

    TaskHandle Task::AddTask(ID id, TaskFunc func, pvoid param, int32 len, pvoid usrPtr, const byte describe[128])
    {
        TaskHandle taskHandle = -1;

        NodeList::Node<TaskNode_t> *taskNode = NULL;

        pthread_mutex_lock(&m_lock);

        taskNode = (NodeList::Node<TaskNode_t> *)(m_freeNodeList.PopFront());

        if (NULL != taskNode)
        {
            taskNode->data.uuid++;

            taskNode->data.param = param;

            taskNode->data.len = len;

            taskNode->data.ownerID = id;

            taskNode->data.func = func;

            taskNode->data.usrPtr = usrPtr;

            if (NULL != describe)
            {
                strcpy(taskNode->data.describe, describe);
            }

            taskHandle = taskNode->data.index;

            taskHandle = (taskHandle << 32) | taskNode->data.uuid;
        }

        pthread_mutex_unlock(&m_lock);

        //条件变量通知

        return taskHandle;
    }
}
