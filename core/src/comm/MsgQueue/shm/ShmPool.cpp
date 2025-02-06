#include "MsgQueue/shm/ShmPool.h"
#include "Log/Log.h"

#include <errno.h>

#define SHM_BLOCK_SIZE (1024 * 1024)
#define SHM_BLOCK_NODE_SIZE (64 * 1024)

namespace LYW_PLUGIN_CORE
{
    ShmPool::ShmPool()
    {
    }

    ShmPool::~ShmPool()
    {
        m_shmPool = NULL;
    }

    ShmPool * ShmPool::GetInstance()
    {
        static ShmPool * self = new(std::nothrow) ShmPool();
        return self;
    }

    eErrCode ShmPool::ShmLock(ShmLock_t &lock)
    {
        if (NULL == m_shmPool)
        {   
            return ERR_SHMPOOL_NOT_INIT;
        }
        pthread_mutex_lock(&lock.lock);
        lock.pid = m_pid;
        return SUC_SUCCESS;
    }

    eErrCode ShmPool::ShmUnLock(ShmLock_t &lock)
    {
        if (NULL == m_shmPool)
        {   
            return ERR_SHMPOOL_NOT_INIT;
        }
        lock.pid = 0;
        pthread_mutex_unlock(&lock.lock);
        return SUC_SUCCESS;
    }

    eErrCode ShmPool::InsertList(ShmBlockList_t &list, ShmBlockListNode_t &basicNode, ShmBlockListNode_t &inSertNode, eListOpt opt)
    {
        //ShmBlockNode_t *headBlockNode = NULL;
        //ShmBlockNode_t *headBlockNodePre = NULL;
        //ShmBlockNode_t *headBlockNodeNext = NULL;
        //ShmBlockNode_t *tailBlockNode = NULL;
        //ShmBlockNode_t *tailBlockNodePre = NULL;
        //ShmBlockNode_t *tailBlockNodeNext = NULL;
        //ShmBlockNode_t *basicBlockNode = NULL;
        //ShmBlockNode_t *basicBlockNodePre = NULL;
        //ShmBlockNode_t *basicBlockNodeNext= NULL;
        //ShmBlockNode_t *insertBlockNode = NULL;
        //ShmBlockNode_t *insertBlockNodePre = NULL;
        //ShmBlockNode_t *insertBlockNodeNext = NULL;
        
        //预处理链接
        if (NULL == m_shmPool)
        {   
            return ERR_SHMPOOL_NOT_INIT;
        }
        
        return SUC_SUCCESS;
    }

    bool ShmPool::IsConnected(const ShmBlockAddr_t &addr)
    {
        if (NULL == m_shmPool)
        {   
            return false;
        }
         
        if (0 == addr.isValid)
        {
            LOG_ERROR("Invalid Shm Address key [%d] index [%d] uuid [%u] offset [%d]\n", addr.key, addr.index, addr.uuid, addr.offset);
            return false;
        }

        //读锁检查是否已连接
        pthread_rwlock_rdlock(&m_lock);
        if (m_blockAddr.IsIndexValid(addr.index) && m_blockAddr[addr.index].shmBlockAddr.uuid == addr.uuid && NULL != m_blockAddr[addr.index].address)
        {
            //块已经链接 返回成功
            pthread_rwlock_unlock(&m_lock);
            return true;
        }
        pthread_rwlock_unlock(&m_lock);

        return false;
    }

    eErrCode ShmPool::ConnectBlock(const ShmBlockAddr_t &addr)
    {
        AddrArrayNode addrArrayNode;

        if (NULL == m_shmPool)
        {   
            return ERR_SHMPOOL_NOT_INIT;
        }
         
        if (0 == addr.isValid)
        {
            LOG_ERROR("Invalid Shm Address key [%d] index [%d] uuid [%u] offset [%d]\n", addr.key, addr.index, addr.uuid, addr.offset);
            return ERR_INVALID_SHMADDR;
        }
        
        //查看是否链接
        if (IsConnected(addr))
        {
            return SUC_SUCCESS;
        }
        
        //需要链接至块
        pthread_rwlock_wrlock(&m_lock);

        //再次确认是否已连接
        if (m_blockAddr.IsIndexValid(addr.index) && m_blockAddr[addr.index].shmBlockAddr.uuid == addr.uuid && NULL != m_blockAddr[addr.index].address)
        {
            pthread_rwlock_unlock(&m_lock);
            return SUC_SUCCESS;
        }

        if (m_blockAddr.IsIndexValid(addr.index) && m_blockAddr[addr.index].shmBlockAddr.uuid != addr.uuid && NULL != m_blockAddr[addr.index].address)
        {
            //断开链接
            if (shmdt(m_blockAddr[addr.index].address) != 0) 
            {
                LOG_ERROR("shmdt failed! errno [%d]\n", errno);
                pthread_rwlock_unlock(&m_lock);
                return ERR_SHM_OPT_FAILD;
            }
            
            m_addrMap.Remove(m_blockAddr[addr.index].address);
        }

        //清理地址表
        m_blockAddr.Erase(addr.index);

        int32 shmid = shmget(addr.key, SHM_BLOCK_NODE_SIZE, 0666);
        if (shmid < 0)
        {
            LOG_ERROR("shmget failed! err[%d]\n", errno);
            pthread_rwlock_unlock(&m_lock);
            return ERR_SHM_OPT_FAILD;
        }

        addrArrayNode.shmBlockAddr = addr;
        addrArrayNode.address = (ShmBlock_t *)shmat(shmid, NULL, 0);

        if (NULL != addrArrayNode.address)
        {
            //更新虚拟地址表
            ShmBlock_t *shmBlock = (ShmBlock_t *)addrArrayNode.address;
            m_addrMap.Insert(addrArrayNode.address, shmBlock->size + sizeof(ShmBlock_t));
            m_blockAddr[addr.key] = addrArrayNode;
        }
        else
        {
            LOG_ERROR("shmgat failed! err[%d]\n", errno);
            pthread_rwlock_unlock(&m_lock);
            return ERR_SHM_OPT_FAILD;
        }

        pthread_rwlock_unlock(&m_lock);

        return SUC_SUCCESS;
    }

    eErrCode ShmPool::DisConnectBlock(const ShmBlockAddr_t &addr)
    {
        if (NULL == m_shmPool)
        {   
            return ERR_SHMPOOL_NOT_INIT;
        }
         
        if (0 == addr.isValid)
        {
            LOG_ERROR("Invalid Shm Address key [%d] index [%d] uuid [%u] offset [%d]\n", addr.key, addr.index, addr.uuid, addr.offset);
            return ERR_INVALID_SHMADDR;
        }


        pthread_rwlock_wrlock(&m_lock);

        if (m_blockAddr.IsIndexValid(addr.index) && m_blockAddr[addr.index].shmBlockAddr.uuid == addr.uuid && NULL != m_blockAddr[addr.index].address)
        {
            //断开链接
            if (shmdt(m_blockAddr[addr.index].address) != 0) 
            {
                LOG_ERROR("shmdt failed! errno [%d]\n", errno);
                pthread_rwlock_unlock(&m_lock);
                return ERR_SHM_OPT_FAILD;
            }
        }
        
        //移除地址映射
        m_addrMap.Remove(m_blockAddr[addr.index].address);
        m_blockAddr.Erase(addr.index);

        pthread_rwlock_unlock(&m_lock);
 
        return SUC_SUCCESS;
    }


    pvoid ShmPool::TransToMmAddr(const ShmBlockAddr_t &shmAddr)
    {
        pvoid ptr = NULL;
        if (0 == shmAddr.isValid)
        {
            return NULL;
        }

        //连接至内存
        if (SUC_SUCCESS != ConnectBlock(shmAddr))
        {
            return NULL;
        }

        pthread_rwlock_rdlock(&m_lock);
        if (m_blockAddr.IsIndexValid(shmAddr.index) && NULL != m_blockAddr[shmAddr.index].address && m_blockAddr[shmAddr.index].address->size > shmAddr.offset)
        {
            ptr = (uint8 *)(m_blockAddr[shmAddr.index].address) + shmAddr.offset;
        }
        pthread_rwlock_unlock(&m_lock);
        return ptr;
    }

}
