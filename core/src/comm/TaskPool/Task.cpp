#include "Task.h"
#include <string>

namespace LYW_PLUGIN_CORE
{
    Task::Task(uint32 maxTaskCount)
    {
        m_maxTaskCount = maxTaskCount;

        m_nodeArray = new(std::nothrow) NodeList::Node<TaskNode_t>[m_maxTaskCount];

        //添加至空闲队列 
        for (uint32 iLoop = 0; iLoop < m_maxTaskCount; iLoop++)
        {
           m_freeNodeList.PushBack(&m_nodeArray[iLoop]);
        }
    }

    Task::~Task()
    {
        if (NULL != m_nodeArray)
        {
            delete [] m_nodeArray;
            m_nodeArray = NULL;
        }
    }



}
