#include "shmPool.h"

#include "Log/Log.h"

#include <errno.h>
#include <unistd.h>
#include <stdint.h>

#define SHM_POOL_FILE_PATH "/dev/zero"

namespace LYW_PLUGIN_CORE
{
    ShmPool::ShmPool(key_t shmPool)
    {
        m_key = ftok(SHM_POOL_FILE_PATH, 0);
        pthread_rwlock_init(&m_lock, NULL);
        Init();
    }

    ShmPool::~ShmPool()
    {
        //当前进程结束才会销毁
        DeInit();
        pthread_rwlock_destroy(&m_lock);
    }

    eErrCode ShmPool::ShmLock(ShmLock_t &lock)
    {
        pthread_mutex_lock(&lock.lock);
        lock.pid = m_pid;
        return SUC_SUCCESS;
    }

    eErrCode ShmPool::ShmUnLock(ShmLock_t &lock)
    {
        lock.pid = 0;
        pthread_mutex_unlock(&lock.lock);
        return SUC_SUCCESS;
    }

    bool ShmPool::IsConnected(const ShmAddr_t &addr)
    {
        if (NULL == m_shmPool)
        {   
            return false;
        }
         
        if (addr.index < 0)
        {
            LOG_ERROR("Invalid Shm Address key [%d] index [%d] uuid [%u] offset [%d]\n", addr.key, addr.index, addr.uuid, addr.offset);
            return false;
        }

        //读锁检查是否已连接
        pthread_rwlock_rdlock(&m_lock);
        if (m_supperBlockAddr.IsIndexValid(addr.index) && m_supperBlockAddr[addr.index].shmSupperBlockAddr.uuid == addr.uuid && NULL != m_supperBlockAddr[addr.index].address)
        {
            //块已经链接 返回成功
            pthread_rwlock_unlock(&m_lock);
            return true;
        }
        pthread_rwlock_unlock(&m_lock);

        return false;
    }

    eErrCode ShmPool::ConnectBlock(const ShmAddr_t &addr)
    {
        AddrArrayNode addrArrayNode;

        if (NULL == m_shmPool)
        {   
            return ERR_SHMPOOL_NOT_INIT;
        }
         
        if (addr.index < 0)
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
        if (m_supperBlockAddr.IsIndexValid(addr.index) && m_supperBlockAddr[addr.index].shmSupperBlockAddr.uuid == addr.uuid && NULL != m_supperBlockAddr[addr.index].address)
        {
            pthread_rwlock_unlock(&m_lock);
            return SUC_SUCCESS;
        }

        if (m_supperBlockAddr.IsIndexValid(addr.index) && m_supperBlockAddr[addr.index].shmSupperBlockAddr.uuid != addr.uuid && NULL != m_supperBlockAddr[addr.index].address)
        {
            //断开链接
            if (shmdt(m_supperBlockAddr[addr.index].address) != 0) 
            {
                LOG_ERROR("shmdt failed! errno [%d]\n", errno);
                pthread_rwlock_unlock(&m_lock);
                return ERR_SHM_OPT_FAILD;
            }
            
            m_mmMap.Remove(m_supperBlockAddr[addr.index].address);
        }

        //清理地址表
        m_supperBlockAddr.Erase(addr.index);

        int32 shmid = shmget(addr.key, SHM_POOL_SUPPER_BLOCK_SIZE, 0666);
        if (shmid < 0)
        {
            LOG_ERROR("shmget failed! err[%d]\n", errno);
            pthread_rwlock_unlock(&m_lock);
            return ERR_SHM_OPT_FAILD;
        }

        addrArrayNode.shmSupperBlockAddr = addr;
        addrArrayNode.address = (ShmSupperBlock_t *)shmat(shmid, NULL, 0);

        if (NULL != addrArrayNode.address)
        {
            //更新虚拟地址表
            ShmBlock_t *shmBlock = (ShmBlock_t *)addrArrayNode.address;
            m_mmMap.Insert(addrArrayNode.address, shmBlock->size + sizeof(ShmBlock_t));
            m_supperBlockAddr[addr.key] = addrArrayNode;
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

    eErrCode ShmPool::DisConnectBlock(const ShmAddr_t &addr)
    {
        if (NULL == m_shmPool)
        {   
            return ERR_SHMPOOL_NOT_INIT;
        }
         
        if (addr.index < 0)
        {
            LOG_ERROR("Invalid Shm Address key [%d] index [%d] uuid [%u] offset [%d]\n", addr.key, addr.index, addr.uuid, addr.offset);
            return ERR_INVALID_SHMADDR;
        }


        pthread_rwlock_wrlock(&m_lock);

        if (m_supperBlockAddr.IsIndexValid(addr.index) && m_supperBlockAddr[addr.index].shmSupperBlockAddr.uuid == addr.uuid && NULL != m_supperBlockAddr[addr.index].address)
        {
            //断开链接
            if (shmdt(m_supperBlockAddr[addr.index].address) != 0) 
            {
                LOG_ERROR("shmdt failed! errno [%d]\n", errno);
                pthread_rwlock_unlock(&m_lock);
                return ERR_SHM_OPT_FAILD;
            }
        }
        
        //移除地址映射
        m_mmMap.Remove(m_supperBlockAddr[addr.index].address);
        m_supperBlockAddr.Erase(addr.index);

        pthread_rwlock_unlock(&m_lock);
 
        return SUC_SUCCESS;
    }

    pvoid ShmPool::TransToMmAddr(const ShmAddr_t &shmAddr)
    {
        pvoid ptr = NULL;

        if (shmAddr.index < 0)
        {
            //直接返回空
            return ptr;
        }

        //连接至内存 -- 已连接的不会额外链接
        if (SUC_SUCCESS != ConnectBlock(shmAddr))
        {
            return ptr;
        }

        pthread_rwlock_rdlock(&m_lock);
        if (m_supperBlockAddr.IsIndexValid(shmAddr.index) && NULL != m_supperBlockAddr[shmAddr.index].address && m_supperBlockAddr[shmAddr.index].address->size > shmAddr.offset)
        {
            ptr = (uint8 *)(m_supperBlockAddr[shmAddr.index].address) + shmAddr.offset;
        }
        pthread_rwlock_unlock(&m_lock);
        return ptr;
    }


    ShmPool::ShmSupperBlock_t * ShmPool::CreateBlock(int32 size)
    {
        int32 index = 0;
        ShmAddr_t shmAddr = {0};
        ShmSupperBlock_t *supperBlock = NULL;
        AddrArrayNode addrArrayNode;

        //连接至内存 -- 已连接的不会额外链接
        if (NULL == m_shmPool)
        {
            LOG_ERROR("shmget Not Init!\n");
            return NULL;
        }
        
        shmAddr.uuid = __sync_fetch_and_add(&m_shmPool->uuid, 1);

        index = m_supperBlockAddr.Size();

        for (int32 iLoop = 0; iLoop < m_supperBlockAddr.Size(); iLoop++)
        {
            if (m_supperBlockAddr.IsIndexValid(iLoop))
            {
                index = iLoop;
                break;
            }
        }

        uint32 uuid = 0;
        while (0 == uuid)
        {
            uuid = __sync_fetch_and_add(&m_shmPool->uuid, 1);
            if (0 == uuid)
            {
                continue;
            }

            shmAddr.key = ftok(SHM_POOL_FILE_PATH, uuid);
            shmAddr.uuid = uuid;

            int32 shmFd = shmget(m_key, sizeof(ShmSupperBlock_t) + size, IPC_CREAT | IPC_EXCL | 0666);
            if (-1 == shmFd)
            {
                LOG_ERROR("shmget failed! [%d]\n", errno);
                uuid = 0;
                continue;
            }
            
            ShmSupperBlock_t *supperBlock = (ShmSupperBlock_t *)shmat(shmFd, NULL, 0);
            
            if (NULL == supperBlock)
            {
                LOG_ERROR("shmat failed! [%d]\n", errno);
                uuid = 0;
                continue;
            }
            

            supperBlock->size = size;
            memcpy(&supperBlock->current.current, &shmAddr, sizeof(shmAddr));

            supperBlock->current.pre.index = -1;
            supperBlock->current.next.index = -1;
        }
        
        //超级块创建成功 申请索引
        ShmLock(m_shmPool->lock);
        //获取一个可用的索引 
        supperBlock->current.current.index = AllocateSupperBlockIndex();
        if (supperBlock->current.current.index < 0) 
        {
            //直接移除
            supperBlock = NULL;
            ShmUnLock(m_shmPool->lock);
            LOG_ERROR("Get Index Failed!\n");
            return NULL;
        }
        else
        {
            ShmPoolList shmNodeList(&m_shmPool->supperBlockList, *this);
            shmNodeList.Push_Back(&supperBlock->current);
        }

        ShmUnLock(m_shmPool->lock);

        //断开当前连接 并重新连接 可以直接进行
        shmdt(supperBlock);

        return (ShmSupperBlock_t *)TransToMmAddr(shmAddr);
    }

    eErrCode ShmPool::Init()
    {
        pthread_rwlock_wrlock(&m_lock);
        if (NULL == m_shmPool)
        {
            int32 shmFd = shmget(m_key, sizeof(ShmPool_t), IPC_CREAT | IPC_EXCL | 0666);
            if (-1 == shmFd)
            {
                if (errno == EEXIST)
                {
                    //已存在 则进行链接
                    shmFd = shmget(m_key, sizeof(ShmPool_t), 0666);
                    if (-1 == shmFd)
                    {
                        pthread_rwlock_unlock(&m_lock);
                        LOG_ERROR("shmget failed! [%d]\n", errno);
                        return ERR_INVALID_SHMADDR;
                    }

                    m_shmPool = (ShmPool_t *)shmat(shmFd, NULL, 0);

                    //等待构建完成
                    while (SHM_POOL_REAY_MAGIC_NUM != m_shmPool->ready)
                    {
                        usleep(10000);
                        LOG_DEBUG("Wait Shm Pool Ready!\n");
                    }
                    pthread_rwlock_unlock(&m_lock);
                }
                else
                {
                    pthread_rwlock_unlock(&m_lock);
                    LOG_ERROR("shmget failed! [%d]\n", errno);
                    return ERR_INVALID_SHMADDR;
                }
            }
            else
            {
                //成功创建 拉起收获进程
                LOG_INFO("Daemon Begin\n", errno);

                srand(time(NULL));

                //初始化共享内存池
                m_shmPool = (ShmPool_t *)shmat(shmFd, NULL, 0);
                memset(m_shmPool, 0x00, sizeof(ShmPool_t));
                //锁
                pthread_mutexattr_t attr;
                pthread_mutexattr_init(&attr);
                pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
                pthread_mutex_init(&m_shmPool->lock.lock, &attr);
                //链表初始化
                m_shmPool->supperBlockList.head.current.index = -1;
                m_shmPool->supperBlockList.tail.current.index = -1;
                m_shmPool->supperBlockList.size = 0;

                m_shmPool->blockBusyList.head.current.index = -1;
                m_shmPool->blockBusyList.tail.current.index = -1;
                m_shmPool->blockBusyList.size = 0;
                for (int32 iLoop = 0; iLoop < SHM_POOL_BLOCK_MAX_EXP; iLoop++)
                {
                    m_shmPool->blockFreeListArray[iLoop].head.current.index = -1;
                    m_shmPool->blockFreeListArray[iLoop].head.current.index = -1;
                    m_shmPool->blockFreeListArray[iLoop].size = 0;
                }

                //设置MAGIC NUMBER
                m_shmPool->ready = SHM_POOL_REAY_MAGIC_NUM;
                m_shmPool->uuid = rand();
                pthread_rwlock_unlock(&m_lock);
            }

            ShmPoolList::List_t *pshmList = NULL;
            ShmAddr_t shmAddr = {0};

            //TransToShmAddr(&m_shmPool->supperBlockList, )
            
            return SUC_SUCCESS;
        }
        else
        {
            LOG_WARN("Shm Has Connected!\n");
            return SUC_SUCCESS;
        }
    }

    eErrCode ShmPool::DeInit()
    {
        pthread_rwlock_wrlock(&m_lock);
        shmdt(m_shmPool);
        m_shmPool = NULL;
        pthread_rwlock_unlock(&m_lock);
        return SUC_SUCCESS;
    }

    int32 ShmPool::AllocateSupperBlockIndex()
    {
        int32 index = 0;

        if (NULL == m_shmPool)
        {
            LOG_ERROR("Shm Pool Not Init!\n");
            return -1;
        }

        ShmPoolList shmList(&m_shmPool->supperBlockList, *this);
        ShmPoolList::Node_t *shmListNode = NULL;

        int32 supperBlockNum = shmList.Size();

        if (supperBlockNum == m_shmPool->index)
        {
            index = __sync_fetch_and_add(&m_shmPool->index, 1);
            return index;
        }
        else if (supperBlockNum < m_shmPool->index)
        {
            //存在空闲索引
            shmListNode = shmList.Front();
            ShmPoolList::Node_t * shmListNode = shmList.Front();
            uint8 *cache = (uint8 *)malloc(supperBlockNum);
            memset(cache, 0x00, supperBlockNum);
            while (NULL != shmListNode)
            {
                shmListNode = shmList.Next(shmListNode);
                if (shmListNode->current.index > 0 && shmListNode->current.index < supperBlockNum)
                {
                    cache[shmListNode->current.index] = 0xFF;
                }
                else
                {
                    LOG_ERROR("Program Bug!\n");
                }
            }

            for (int32 iLoop = 0; iLoop < supperBlockNum; iLoop++)
            {
                if (0x00 == cache[iLoop])
                {
                    index = iLoop;
                    break;
                }
            }
        }
        else
        {
            LOG_ERROR("Program Bug!\n");
        }

        return index;
    }

    eErrCode ShmPool::DestroyBlock(ShmAddr_t &shmAddr)
    {
        int32 shmid = shmget(shmAddr.key, 0, 0666);

        if (-1 != shmid)
        {
            if (-1 == shmctl(shmid, IPC_RMID, NULL)) 
            {
                LOG_ERROR("shm key [%d] shmctl failed! [%d]\n", errno);
                return ERR_FAILED;
            }
        }
        else
        {
            LOG_ERROR("shm key [%d] shmget failed! [%d]\n", errno);
            return ERR_FAILED;
        }

        return SUC_SUCCESS;
    }

    pvoid ShmPool::Malloc(int32 size)
    {
        pvoid ptr = NULL;


        if (size <= 0)
        {
            return ptr;
        }

        if (NULL == m_shmPool || size <= 0)  
        {
            LOG_ERROR("shmPool Not Init!\n");
            return ptr;
        }


        //计算size指数
        int exp = Exponent((size + 1) / 8192);

        ShmLock(m_shmPool->lock);
            ShmPoolList freeList(&m_shmPool->blockFreeListArray[exp], *this);
            ShmPoolList busyList(&m_shmPool->blockBusyList, *this);
        while (true)
        {
            if (0 != freeList.Size())
            {
                ShmBlock_t *blk = (ShmBlock_t *)freeList.Front();
                if (blk->leftSize >= sizeof(ShmNode_t) + size)
                {
                    ptr = blk->nodeArray + (blk->size - blk->leftSize) + sizeof(ShmNode_t);
                    blk->reCount++;
                    break;
                }

                //归还

                continue;
            }

            //申请
        }
        ShmUnLock(m_shmPool->lock);
        return ptr;
    }

    int32 ShmPool::Exponent(uint32 num)
    {
        int32 pos = sizeof(num) * 8 - 1 - __builtin_clz(num);

        if (0 == num)
        {
            return 0;
        }

        if (0 != (num & (~(1ull << pos))))
        {
            return pos + 1;
        }
        else
        {
            return pos;
        }
    }
}
