/**
 * @details     共享缓存池
 *              
 * @brief       共享缓存池
 * @file        
 *
 * @version     1.0
 * @author      lyw
 * @date        2025年2月5日
 */

#ifndef __LYW_PLUGIN_CORE_MSQ_QUEUE_SHM_POOL1_FILE_H__
#define __LYW_PLUGIN_CORE_MSQ_QUEUE_SHM_POOL1_FILE_H__

#include "MsgQueue/shm/shmPoolCommDef.h"
#include "MsgQueue/shm/shmPoolList.h"
#include "DynamicArray/DynamicArray.hpp"
#include "MsgQueue/shm/AddrMap.hpp"

#include <map>

#define SHM_POOL_BLOCK_SIZE (8192)
#define SHM_POOL_SUPPER_BLOCK_SIZE (1 * 1024 * 1024)
#define SHM_POOL_BLOCK_MAX_EXP (16)
#define SHM_POOL_REAY_MAGIC_NUM 0x41312267

namespace LYW_PLUGIN_CORE
{
    class _VISIBLE_ ShmPool : public ShmPoolCommDef
    {
        private:

        public:
            /**
             * @brief               共享内存锁
             */
            typedef struct __attribute__((aligned(4))) tag_ShmLock
            {
                pthread_mutex_t lock;   ///< 进程互斥量
                pid_t pid;              ///< 持有进程ID -- 防止进程崩溃出现锁无法释放的情况
            } ShmLock_t;

            /**
             * @brief                   内存池
             */
            typedef struct __attribute__((aligned(4))) tag_ShmPool
            {
                uint32 ready; ///< 0x31323334
                ShmLock_t lock; ///< 操作锁
                int32 index; ///< 块键值索引 新增 Block 键值为 key + index
                int32 uuid; ///< 块键值索引 新增 Block 键值为 key + index
                ShmPoolList::List_t supperBlockList;
                ShmPoolList::List_t blockBusyList; ///< 占用块
                ShmPoolList::List_t blockFreeListArray[SHM_POOL_BLOCK_MAX_EXP]; ///< 空闲块
            } ShmPool_t;

            /**
             * @brief                   超级块
             */
            typedef struct __attribute__((aligned(4))) tag_ShmSupperBlock
            {
                //链表
                ShmPoolList::Node_t current;
                uint32 size; ///< 超级块大小
                bool isValid; ///< 是否可用
                uint8 blockArray[0]; ///< ShmBlock_t 
            } ShmSupperBlock_t;


            /**
             * @brief                   块
             */
            typedef struct __attribute__((aligned(4))) tag_ShmBlock
            {
                ShmPoolList::Node_t current;
                ShmPoolList::List_t nodeList;
                uint32 size; ///< 块大小
                uint32 leftSize; ///< 剩余可分配大小
                uint32 reCount; ///< 引用计数
                uint8 nodeArray[0]; ///< ShmNode_t
            } ShmBlock_t;


            /**
             * @brief                   节点
             */
            typedef struct __attribute__((aligned(4))) tag_ShmNode
            {
                uint32 offset; ///< 距离 ShmBlock_t 的偏移量
                uint8 data[0];
            } ShmNode_t;

            /**
             * @brief               地址映射表节点
             */
            class AddrArrayNode
            { 
                public:
                    ShmAddr_t shmSupperBlockAddr; ///< 块地址
                    ShmSupperBlock_t *address; ///< 虚拟地址

                    /**
                     * @brief       构造
                     */
                    AddrArrayNode()
                    {
                        shmSupperBlockAddr.index = -1;
                        shmSupperBlockAddr.uuid = 0;
                        shmSupperBlockAddr.key = 0;
                        shmSupperBlockAddr.offset = 0;
                        address = NULL;
                    }
            };

        public:
            /**
             * @brief           构造
             */
            ShmPool(key_t shmPool);

            /**
             * @brief           析构
             */
            ~ShmPool();

            /**
             * @brief           虚拟地址转换为共享缓存地址
             *
             * @param ptr       虚拟地址
             *
             * @return 错误码
             */
            eErrCode TransToShmAddr(pvoid ptr, ShmAddr_t &shmAddr);

            /**
             * @brief           共享缓存地址转换为虚拟地址
             *
             * @param shmAddr   共享缓存地址
             *
             * @return 虚拟地址 NULL 失败
             */
            pvoid TransToMmAddr(const ShmAddr_t &shmAddr);

            /**
             * @brief           申请一段共享缓存
             *
             * @param size      大小
             *
             * @return 地址
             */
            pvoid Malloc(int32 size);

            /**
             * @brief           释放一段共享缓存
             *
             * @param ptr       指针
             */
            void Free(pvoid ptr);


        private:
            /**
             * @brief               锁
             *
             * @param lock          锁结构
             *
             * @return 错误码
             */
            eErrCode ShmLock(ShmLock_t &lock);

            /**
             * @brief               解锁
             *
             * @param lock          锁结构
             *
             * @return 错误码
             */
            eErrCode ShmUnLock(ShmLock_t &lock);

            /**
             * @brief           是否已经链接
             *
             * @param addr      共享缓存地址
             *
             * @return true 已连接 false 未链接
             */
            bool IsConnected(const ShmAddr_t &addr);

            /**
             * @brief           链接至一个块
             *
             * @param addr      共享缓存地址
             *
             * @return 错误码
             */
            eErrCode ConnectBlock(const ShmAddr_t &addr);

            /**
             * @brief           断开共享内存块链接
             *
             * @param addr      共享内存地址
             *
             * @return 错误码
             */
            eErrCode DisConnectBlock(const ShmAddr_t &addr);

            /**
             * @brief           初始化
             *
             * @return 错误码
             */
            eErrCode Init();

            /**
             * @brief           反初始化
             *
             * @return 错误码
             */
            eErrCode DeInit();

            /**
             * @brief           创建一个块
             *
             * @param size      大小 
             *
             * @return 超级块虚拟地址 
             */
            ShmSupperBlock_t * CreateBlock(int32 size = SHM_POOL_SUPPER_BLOCK_SIZE);

            /**
             * @brief               移除超级块
             *
             * @param shmAddr       共享内存地址
             *
             * @return 错误码
             */
            eErrCode DestroyBlock(ShmAddr_t &shmAddr);
            /**
             * @brief           申请一个超级块索引 非进程安全 
             *
             * @return 超级块索引
             */
            int32 AllocateSupperBlockIndex(void);

            /**
             * @brief           计算num的2的幂 2^(n - 1) < num <= 2^n 
             *
             * @param num       计算2的幂次
             *
             * @return 幂次
             */
            int32 Exponent(int32 num);

        private:
            pid_t m_pid; ///< 当前进程pid

            key_t m_key; ///< head 键值

            ShmPool_t *m_shmPool; ///< 共享缓存头

            DynamicArray <AddrArrayNode> m_supperBlockAddr;   ///< 块地址

            AddrMap m_mmMap;  ///< 地址map 用于快速查找 块的起始地址（使用共享内存块中的任意地址查询 返回块的起始地址与块的信息）

            pthread_rwlock_t m_lock;    ///< 资源锁
    };
}
#endif
