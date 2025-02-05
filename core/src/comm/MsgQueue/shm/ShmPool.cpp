#include "MsgQueue/shm/ShmPool.h"
#include "Log/Log.h"

#define SHM_BLOCK_SIZE (512 * 1024)

namespace LYW_PLUGIN_CORE
{
    ShmPool::ShmPool()
    {
        m_shmHead = NULL;
    }

    ShmPool::~ShmPool()
    {
        m_shmHead = NULL;
    }

    ShmPool * ShmPool::GetInstance()
    {
        static ShmPool * self = new(std::nothrow) ShmPool();
        return self;
    }

    pvoid ShmPool::Malloc(int32 size)
    {
        key_t key = 0;
        std::map<key_t, Block_t *>::iterator it;
        Block_t *blk = NULL;

        if (size <= 0 || NULL == m_shmHead)
        {
            return NULL;
        }

        while (true)
        {

            if (size >= SHM_BLOCK_SIZE)
            {
                //创建大块
                if ((blk = CreateBlock(size)) == NULL)
                {
                    LOG_ERROR("Shm Create Failed!\n");
                    return NULL;
                }

                //添加至占用队列
            }

            //脏读 可分配块
            key = m_shmHead->b_FreeBlock;
            it = m_blockMap.find(key);

            if (it == m_blockMap.end())
            {
                //先尝试链接
                ConnectBlock(key);
                continue;
            }
            
            ShmLock();
            if (key != m_shmHead->b_FreeBlock)
            {
                ShmUnLock();
                continue;
            }
            
            //分配
            ShmUnLock();
        }
        return NULL;
    }

}
