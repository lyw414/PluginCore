#include "MsgQueue/MsgQueue.h"
#include "Log/Log.h"

namespace LYW_PLUGIN_CORE
{
    MsgQueue::Handle MsgQueue::Connect(const byte szInfo[256])
    {
        if (NULL == m_msgQueue)
        {
            LOG_ERROR("Invalid Msg Queue!\n");
            return ERR_INVALID_MSGQUEUE;
        }

        return m_msgQueue->Connect(szInfo);
    }

    eErrCode MsgQueue::DisConnect(Handle handle)
    {
        if (NULL == m_msgQueue)
        {
            LOG_ERROR("Invalid Msg Queue!\n");
            return ERR_INVALID_MSGQUEUE;
        }

        return m_msgQueue->DisConnect(handle);
    }

    MsgQueue::Session MsgQueue::Pub(ID id, EventID eventID, pvoid msg, int32 msgLen)
    {
        if (NULL == m_msgQueue)
        {
            LOG_ERROR("Invalid Msg Queue!\n");
            return ERR_INVALID_MSGQUEUE;
        }

        return m_msgQueue->Pub(id, eventID, msg, msgLen);
    }

    MsgQueue::SubHandle MsgQueue::Sub(ID id, EventID eventID, SubFunc subFunc, pvoid usrPtr)
    {
        if (NULL == m_msgQueue)
        {
            LOG_ERROR("Invalid Msg Queue!\n");
            return ERR_INVALID_MSGQUEUE;
        }

        return m_msgQueue->Sub(id, eventID, subFunc, usrPtr);
    }

    eErrCode MsgQueue::UnSubBySubHandle(SubHandle handle)
    {
        if (NULL == m_msgQueue)
        {
            LOG_ERROR("Invalid Msg Queue!\n");
            return ERR_INVALID_MSGQUEUE;
        }

        return m_msgQueue->UnSubBySubHandle(handle);
    }

    eErrCode MsgQueue::UnSub(Handle handle)
    {
        if (NULL == m_msgQueue)
        {
            LOG_ERROR("Invalid Msg Queue!\n");
            return ERR_INVALID_MSGQUEUE;
        }

        return m_msgQueue->UnSub(handle);
    }

    eErrCode MsgQueue::Wait(Session session, int32 timeout)
    {
        if (NULL == m_msgQueue)
        {
            LOG_ERROR("Invalid Msg Queue!\n");
            return ERR_INVALID_MSGQUEUE;
        }

        return m_msgQueue->Wait(session, timeout);
    }
}

