#include "MsgQueue/shm/shmPoolList.h"
#include "MsgQueue/shm/shmPool.h"

namespace LYW_PLUGIN_CORE
{
    ShmPoolList::ShmPoolList(List_t *list, ShmPool &shmPool) : m_shmPool(shmPool)
    {
        m_list = list;
    }

    ShmPoolList::~ShmPoolList()
    {
    }

    ShmPoolList::Node_t * Pop_front()
    {
        return NULL;
    }

    ShmPoolList::Node_t * Front()
    {
        return NULL;
    }

    ShmPoolList::Node_t * ShmPoolList::Next(Node_t *node)
    {
        if (NULL == m_list)
        {
            return NULL;
        }
        return NULL;
    }

    ShmPoolList::Node_t * ShmPoolList::Pre(Node_t *node)
    {
        if (NULL == m_list)
        {
            return NULL;
        }
 
        return NULL;
    }

    int32 ShmPoolList::Size()
    {
        if (NULL == m_list)
        {
            return 0;
        }
 
        return m_list->size;
    }
}
